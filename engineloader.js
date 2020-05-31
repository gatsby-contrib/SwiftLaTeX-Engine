var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
var EngineStatus;
(function (EngineStatus) {
    EngineStatus[EngineStatus["Init"] = 1] = "Init";
    EngineStatus[EngineStatus["Ready"] = 2] = "Ready";
    EngineStatus[EngineStatus["Busy"] = 3] = "Busy";
    EngineStatus[EngineStatus["Error"] = 4] = "Error";
})(EngineStatus || (EngineStatus = {}));
var ENGINE_PATH = 'swiftlatex.js';
var CompileResult = /** @class */ (function () {
    function CompileResult() {
        this.pdf = undefined;
        this.status = -254;
        this.log = 'No log';
    }
    return CompileResult;
}());
var LaTeXEngine = /** @class */ (function () {
    function LaTeXEngine() {
        this.latexWorker = undefined;
        this.latexWorkerStatus = EngineStatus.Init;
    }
    LaTeXEngine.prototype.loadEngine = function () {
        return __awaiter(this, void 0, void 0, function () {
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.latexWorker !== undefined) {
                            throw new Error('Other instance is running, abort()');
                        }
                        this.latexWorkerStatus = EngineStatus.Init;
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker = new Worker(ENGINE_PATH);
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    if (cmd === 'postRun') {
                                        _this.latexWorkerStatus = EngineStatus.Ready;
                                        resolve();
                                    }
                                    else {
                                        _this.latexWorkerStatus = EngineStatus.Error;
                                        reject();
                                    }
                                };
                            })];
                    case 1:
                        _a.sent();
                        this.latexWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXEngine.prototype.compileLaTeX = function () {
        return __awaiter(this, void 0, void 0, function () {
            var start_compile_time, res;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.latexWorkerStatus !== EngineStatus.Ready) {
                            console.log('Engine is still spinning.');
                            return [2 /*return*/];
                        }
                        this.latexWorkerStatus = EngineStatus.Busy;
                        start_compile_time = performance.now();
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    var result = data['result'];
                                    var log = data['log'];
                                    var status = data['status'];
                                    var pdf = data['pdf'];
                                    if (cmd === 'compile') {
                                        _this.latexWorkerStatus = EngineStatus.Ready;
                                        if (result === 'failed') {
                                            console.error('Engine crushed terribly. Log: ' + log);
                                            _this.latexWorkerStatus = EngineStatus.Error;
                                            reject(log);
                                        }
                                        else {
                                            console.log("Engine compilation finish " + (performance.now() - start_compile_time));
                                            var nice_report = new CompileResult();
                                            nice_report.status = status;
                                            nice_report.log = log;
                                            nice_report.pdf = pdf;
                                            resolve(nice_report);
                                        }
                                    }
                                };
                                _this.latexWorker.postMessage({ 'cmd': 'compilelatex' });
                            })];
                    case 1:
                        res = _a.sent();
                        this.latexWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/, res];
                }
            });
        });
    };
    /* Internal Use */
    LaTeXEngine.prototype.compileFormat = function () {
        return __awaiter(this, void 0, void 0, function () {
            var start_compile_time;
            var _this = this;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (this.latexWorkerStatus !== EngineStatus.Ready) {
                            console.log('Engine is still spinning or not loaded.');
                            return [2 /*return*/];
                        }
                        this.latexWorkerStatus = EngineStatus.Busy;
                        start_compile_time = performance.now();
                        return [4 /*yield*/, new Promise(function (resolve, reject) {
                                _this.latexWorker.onmessage = function (ev) {
                                    var data = ev['data'];
                                    var cmd = (data['cmd']);
                                    if (cmd === 'compile') {
                                        if (data['status'] === 0) {
                                            var formatArray = data['pdf']; /* PDF for result */
                                            var formatBlob = new Blob([formatArray], { type: 'application/octet-stream' });
                                            var formatURL_1 = URL.createObjectURL(formatBlob);
                                            setTimeout(function () { URL.revokeObjectURL(formatURL_1); }, 30000);
                                            console.log('Download format file via ' + formatURL_1);
                                        }
                                        else {
                                            reject(data['log']);
                                        }
                                    }
                                };
                                _this.latexWorker.postMessage({ 'cmd': 'compileformat' });
                            })];
                    case 1:
                        _a.sent();
                        console.log("It takes $(performance.now - start_compile_time) ms");
                        this.latexWorker.onmessage = function (ev) {
                        };
                        return [2 /*return*/];
                }
            });
        });
    };
    LaTeXEngine.prototype.setEngineMainFile = function (filename) {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'setmainfile', 'url': filename });
        }
    };
    LaTeXEngine.prototype.writeMemFSFile = function (srccode, filename) {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'writefile', 'url': filename, 'src': srccode });
        }
    };
    LaTeXEngine.prototype.makeMemFSFolder = function (folder) {
        if (this.latexWorker !== undefined) {
            if (folder === '' || folder === '/')
                return;
            this.latexWorker.postMessage({ 'cmd': 'mkdir', 'url': folder });
        }
    };
    LaTeXEngine.prototype.flushCache = function () {
        if (this.latexWorker !== undefined) {
            console.warn("Flushing");
            this.latexWorker.postMessage({ 'cmd': 'flushcache' });
        }
    };
    LaTeXEngine.prototype.closeWorker = function () {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'grace' });
            this.latexWorker = undefined;
        }
    };
    return LaTeXEngine;
}());
