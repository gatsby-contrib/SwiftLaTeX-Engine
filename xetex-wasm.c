// First of all, remove pdf_files_init/close from xetex-ini.c
// And remove picture handling functions from xetex-pic.c
#include "core-bridge.h"
#include <md5.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "core-memory.h"
void issue_warning(void *context, char const *text) {
    printf("%s\n", text);
}

void issue_error(void *context, char const *text) {
    printf("%s\n", text);
}

int get_file_md5(void *context, char const *path, char *digest) {

    md5_state_t state;
#define FILE_BUF_SIZE 128
    char file_buf[FILE_BUF_SIZE];
    size_t read;
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "This file does not exists %s\n, so returning an empty md5",
                path);
        return -1;
    }

    md5_init(&state);
    while ((read = fread(&file_buf, sizeof(char), FILE_BUF_SIZE, f)) > 0) {
        md5_append(&state, (const md5_byte_t *)file_buf, read);
    }
    md5_finish(&state, digest);
    fclose(f);
#undef FILE_BUF_SIZE
    return 0;
}
int get_data_md5(void *context, char const *data, size_t len, char *digest) {

    md5_state_t state;
    md5_init(&state);
    md5_append(&state, data, len);
    md5_finish(&state, digest);
    return 0;
}


void *output_open(void *context, char const *path, int is_gz) {
    return fopen(path, "w");
}

void *output_open_stdout(void *context) { return stdout; }

int output_putc(void *context, void *handle, int c) {
    // printf("output_putc\n");
    return fputc(c, handle);
}

size_t output_write(void *context, void *handle, const char *data, size_t len) {
    // printf("output_write\n");
    return fwrite(data, 1, len, handle);
}

int output_flush(void *context, void *handle) {
    // printf("output_fflush\n");
    return fflush(handle);
}

int output_close(void *context, void *handle) {
    return fclose(handle);
}

#define MAXAVAILABLENAMESIZE 512


#define TEXCACHEROOT "/tex/"
int kpse_fetch_from_network(char *name_ret);
static int existsInCacheDir(char *name)
{
    char kpseurl[MAXAVAILABLENAMESIZE] = {0};
    snprintf(kpseurl, MAXAVAILABLENAMESIZE, "%s%s", TEXCACHEROOT, name);
    kpseurl[MAXAVAILABLENAMESIZE - 1] = 0;
    if ( access( kpseurl, F_OK ) != -1 ) {
        strncpy(name, kpseurl, MAXAVAILABLENAMESIZE);
        name[MAXAVAILABLENAMESIZE - 1] = 0;
        return 1;
    }
    //printf("file no there %s\n", kpseurl);
    return -1;
}


static void fix_extension(char *patched_name, tt_input_format_type format)
{
#define SUFFIX(suf) strcat(patched_name, suf);

    switch (format) {
    case TTIF_TFM:
        SUFFIX(".tfm");
        break;
    case TTIF_AFM:
        SUFFIX(".afm");
        break;
    case TTIF_BIB:
        SUFFIX(".bib");
        break;
    case TTIF_BST:
        SUFFIX(".bst");
        break;
    case TTIF_CNF:
        SUFFIX(".cnf");
        break;
    case TTIF_FORMAT:
        SUFFIX(".fmt");
        break;
    case TTIF_FONTMAP:
        SUFFIX(".fontmap");
        break;
    case TTIF_OFM:
        SUFFIX(".ofm");
        break;
    case TTIF_OVF:
        SUFFIX(".ovf");
        break;
    case TTIF_TEX:
        SUFFIX(".tex");
        break;
    case TTIF_TYPE1:
        SUFFIX(".pfb");
        break;
    case TTIF_VF:
        SUFFIX(".vf");
        break;
    case TTIF_TRUETYPE:
        SUFFIX(".ttf");
        break;
    case TTIF_ENC:
        SUFFIX(".enc");
        break;
    case TTIF_CMAP:
        SUFFIX(".cmap");
        break;
    case TTIF_SFD:
        SUFFIX(".sfd");
        break;
    case TTIF_OPENTYPE:
        SUFFIX(".otf");
        break;
    default:
        fprintf(stderr, "Unknown format\n");
        break;
    }
#undef SUFFIX

}

char *kpse_find_file(const char *name, tt_input_format_type format) {

    if (name == NULL) {
        return NULL;
    }

    if (strlen(name) > MAXAVAILABLENAMESIZE / 2) {
        return NULL;
    }

    char* patched_name = xmalloc(MAXAVAILABLENAMESIZE);

    memset(patched_name, 0, MAXAVAILABLENAMESIZE);

    strcat(patched_name, name);

    if (access(patched_name, F_OK) != -1) {
        return patched_name;
    }

    if (strrchr(patched_name, '.') == NULL) {
        fix_extension(patched_name, format);
    }

    //Try again2
    if (access(patched_name, F_OK) != -1) {
        return patched_name;
    }


    if (existsInCacheDir(patched_name) == 1)
    {
        return patched_name;
    }

    if (kpse_fetch_from_network(patched_name) ==
            0)
    {

        //Recheck the cache
        if (existsInCacheDir(patched_name) == 1) {
            // printf("==>%s\n", patched_name);
            return patched_name;
        }
        // printf("==>failed\n");

    }

    free(patched_name); // We try out bese, just leave it
    return NULL;
}

void *input_open(void *context, char const *path, tt_input_format_type format,
                 int is_gz) {
    // fprintf(stderr, "Opening %s format %d\n", path, format);
    char *normalized_path = kpse_find_file(path, format);
    if (normalized_path != NULL) {
        FILE *res = fopen(normalized_path, "rb");
        free(normalized_path);
        return res;
    } else {
        return NULL;
    }
}

size_t input_get_size(void *context, void *handle) {
    int fpno = fileno(handle);
    struct stat st;
    fstat(fpno, &st);
    return st.st_size;
}

size_t input_seek(void *context, void *handle, ssize_t offset, int whence,
                  int *internal_error) {

    int seek_res = fseek(handle, offset, whence);
    if (seek_res != 0) {
        fprintf(stderr, "seek failed  %d\n", seek_res);
        *internal_error = 1;
        return -1;
    } else {
        if (whence == SEEK_SET) {
            return offset;
        }
        /* Return current file pointer */
        return ftell(handle);
    }
}

ssize_t input_read(void *context, void *handle, char *data, size_t len) {
    // printf("input_read\n");
    return fread(data, 1, len, handle);
}

int input_getc(void *context, void *handle) {
    // printf("input_getc\n");
    return fgetc(handle);
}

int input_ungetc(void *context, void *handle, int ch) {

    return ungetc(ch, handle);
}

int input_close(void *context, void *handle) { return fclose(handle); }


tt_bridge_api_t ourapi;
char MAIN_ENTRY_FILE[512];

int compileLaTeX() {
    /* Compatibility Hack */
    printf("This is SwiftLaTeX, Version 2.13 (TeX Live 2019/Gentoo) (preloaded format=swiftlatex)\n");
    printf(" restricted \\write18 enabled.\n");
    printf("entering extended mode\n");
    return tex_simple_main(&ourapi, "swiftlatex.fmt", MAIN_ENTRY_FILE, time(0), 0);
}

int compileFormat() {
    return tex_simple_main(&ourapi, "xelatex.fmt", "xelatex.ini", time(0), 1); //xelatex.fmt does not matter
}

int compileBibtex() {
    char main_aux_file[512];
    strncpy(main_aux_file, MAIN_ENTRY_FILE, 512);
    int len = strlen(main_aux_file);
    if(len < 3) return -1;
    main_aux_file[len - 1] = 'x';
    main_aux_file[len - 2] = 'u';
    main_aux_file[len - 3] = 'a';
    return bibtex_simple_main(&ourapi, main_aux_file);
}

int compilePDF(){
    char main_xdv_file[512];
    strncpy(main_xdv_file, MAIN_ENTRY_FILE, 512);
    int len = strlen(main_xdv_file);
    if(len < 3) return -1;
    main_xdv_file[len - 1] = 'v';
    main_xdv_file[len - 2] = 'd';
    main_xdv_file[len - 3] = 'x';
    return dvipdfmx_simple_main(&ourapi, main_xdv_file, "result.pdf", false, false, time(0));
  // return tex_simple_main(&ourapi, "xelatex.fmt", "xelatex.ini", time(0), 1); //xelatex.fmt does not matter
}

int setMainEntry(const char *p) {
    strncpy(MAIN_ENTRY_FILE, p, 512);
    MAIN_ENTRY_FILE[511] = 0;
    return 0;
}

int main() {
    ourapi.issue_warning = issue_warning;
    ourapi.issue_error = issue_error;
    ourapi.get_file_md5 = get_file_md5;
    ourapi.get_data_md5 = get_data_md5;
    ourapi.output_open = output_open;
    ourapi.output_open_stdout = output_open_stdout;
    ourapi.output_putc = output_putc;
    ourapi.output_write = output_write;
    ourapi.output_close = output_close;
    ourapi.output_flush = output_flush;
    ourapi.input_open = input_open;
    ourapi.input_get_size = input_get_size;
    ourapi.input_seek = input_seek;
    ourapi.input_read = input_read;
    ourapi.input_getc = input_getc;
    ourapi.input_ungetc = input_ungetc;
    ourapi.input_close = input_close;
    strncpy(MAIN_ENTRY_FILE, "main.tex", 512);

    return 0;
}



