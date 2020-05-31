CC=emcc
CXX=em++
DEBUGFLAGS = -O3
CFLAGS = $(DEBUGFLAGS) -Igraphite2/include -Iharfbuzz/include -s USE_FREETYPE=1 -s USE_HARFBUZZ=1 -s USE_ICU=1 -s USE_LIBPNG=1
LDFLAGS =  $(DEBUGFLAGS)  --js-library library.js -s USE_FREETYPE=1 \
 -s USE_HARFBUZZ=1 -s USE_LIBPNG=1 --pre-js pre.js \
 -s EXPORTED_FUNCTIONS='["_compileBibtex", "_compileLaTeX", "_compileFormat", "_compilePDF", "_setMainEntry", "_main"]' -s NO_EXIT_RUNTIME=1 \
  -s WASM=1 -s ALLOW_MEMORY_GROWTH=1
CXX_LINK = $(CXX) -o $@ $(LDFLAGS) 

csources = xetex-io.c xetex-xetex0.c xetex-ini.c xetex-texmfmp.c xetex-errors.c \
xetex-scaledmath.c xetex-output.c xetex-stringpool.c core-memory.c core-bridge.c \
xetex-pagebuilder.c xetex-shipout.c xetex-math.c xetex-ext.c xetex-linebreak.c \
 xetex-pic.c bibtex.c md5.c xetex-wasm.c miniz.c \
dpx-agl.c         dpx-dvi.c        dpx-pdfdoc.c       dpx-spc_color.c     dpx-tt_aux.c \
dpx-bmpimage.c    dpx-dvipdfmx.c   dpx-pdfdraw.c      dpx-spc_dvipdfmx.c  dpx-tt_cmap.c \
dpx-cff.c         dpx-epdf.c       dpx-pdfencoding.c  dpx-spc_dvips.c     dpx-tt_glyf.c \
dpx-cff_dict.c    dpx-error.c      dpx-pdfencrypt.c   dpx-spc_html.c      dpx-tt_gsub.c \
dpx-cid.c         dpx-fontmap.c    dpx-pdffont.c      dpx-spc_misc.c      dpx-tt_post.c \
dpx-cidtype0.c    dpx-jp2image.c   dpx-pdfnames.c     dpx-spc_pdfm.c      dpx-tt_table.c \
dpx-cidtype2.c    dpx-jpegimage.c  dpx-pdfobj.c       dpx-spc_tpic.c      dpx-type0.c \
dpx-cmap.c        dpx-mem.c        dpx-pdfparse.c     dpx-spc_util.c      dpx-type1.c \
dpx-cmap_read.c   dpx-mfileio.c    dpx-pdfresource.c  dpx-spc_xtx.c       dpx-type1c.c \
dpx-cmap_write.c  dpx-mpost.c      dpx-pdfximage.c    dpx-specials.c      dpx-unicode.c \
dpx-cs_type2.c    dpx-numbers.c    dpx-pkfont.c       dpx-subfont.c       dpx-vf.c \
dpx-dpxconf.c     dpx-otl_conf.c   dpx-pngimage.c     dpx-t1_char.c \
dpx-dpxcrypt.c    dpx-otl_opt.c    dpx-pst.c          dpx-t1_load.c \
dpx-dpxfile.c     dpx-pdfcolor.c   dpx-pst_obj.c      dpx-tfm.c \
dpx-dpxutil.c     dpx-pdfdev.c     dpx-sfnt.c         dpx-truetype.c
 
cppsources = teckit-Engine.cpp xetex-XeTeXOTMath.cpp xetex-XeTeXLayoutInterface.cpp \
 xetex-XeTeXFontMgr.cpp xetex-XeTeXFontInst.cpp xetex-XeTeXFontMgr_FC.cpp

headers = xetex-XeTeXFontMgr.h xetex-XeTeXLayoutInterface.h xetex-XeTeXOTMath.h xetex-XeTeXFontMgr_FC.h \
			xetex-XeTeXFontInst.h xetex-xetexd.h xetex-web.h xetex-swap.h xetex-stringpool.h xetex-ext.h \
			xetex-core.h xetex-constants.h teckit-Format.h teckit-cxx-Engine.h teckit-Compiler.h teckit-Common.h \
			teckit-c-Engine.h md5.h core-memory.h core-strutils.h core-foundation.h core-bridge.h bibtex.h \
			dpx-agl.h           dpx-cs_type2.h   dpx-otl_conf.h     dpx-pst.h           dpx-tfm.h \
dpx-bmpimage.h      dpx-dpxconf.h    dpx-otl_opt.h      dpx-pst_obj.h       dpx-truetype.h \
dpx-cff_dict.h      dpx-dpxcrypt.h   dpx-pdfcolor.h     dpx-sfnt.h          dpx-tt_aux.h \
dpx-cff.h           dpx-dpxfile.h    dpx-pdfdev.h       dpx-spc_color.h     dpx-tt_cmap.h \
dpx-cff_limits.h    dpx-dpxutil.h    dpx-pdfdoc.h       dpx-spc_dvipdfmx.h  dpx-tt_glyf.h \
dpx-cff_stdstr.h    dpx-dvicodes.h   dpx-pdfdraw.h      dpx-spc_dvips.h     dpx-tt_gsub.h \
dpx-cff_types.h     dpx-dvi.h        dpx-pdfencoding.h  dpx-spc_html.h      dpx-tt_post.h \
dpx-cid_basefont.h  dpx-dvipdfmx.h   dpx-pdfencrypt.h   dpx-spc_misc.h      dpx-tt_table.h \
dpx-cid.h           dpx-epdf.h       dpx-pdffont.h      dpx-spc_pdfm.h      dpx-type0.h \
dpx-cid_p.h         dpx-error.h      dpx-pdflimits.h    dpx-spc_tpic.h      dpx-type1c.h \
dpx-cidtype0.h      dpx-fontmap.h    dpx-pdfnames.h     dpx-spc_util.h      dpx-type1.h \
dpx-cidtype2.h      dpx-jp2image.h   dpx-pdfobj.h       dpx-spc_xtx.h       dpx-unicode.h \
dpx-cmap.h          dpx-jpegimage.h  dpx-pdfparse.h     dpx-specials.h      dpx-vf.h \
dpx-cmap_p.h        dpx-mem.h        dpx-pdfresource.h  dpx-subfont.h \
dpx-cmap_read.h     dpx-mfileio.h    dpx-pdfximage.h    dpx-system.h \
dpx-cmap_write.h    dpx-mpost.h      dpx-pkfont.h       dpx-t1_char.h \
dpx-core.h          dpx-numbers.h    dpx-pngimage.h     dpx-t1_load.h \


cobjects = $(csources:.c=.o)

cppobjects = $(cppsources:.cpp=.o)

swiftlatex.js: $(headers) $(cobjects) $(cppobjects) 
	$(CXX_LINK) $(cobjects) $(cppobjects)

$(cobjects): %.o: %.c
	$(CC) -c $(CFLAGS) -I. $< -o $@

$(cppobjects): %.o: %.cpp
	$(CXX) -std=c++17 -c $(CFLAGS) -I.  $< -o $@

clean:
	rm -f *.o  swiftlatex*
	
