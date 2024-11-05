/**
 * 所有插件以全局方式安装
 * 系统环境变量设置 NODE_PATH=你的node_modules目录
 * 
 * 第三方插件依赖:
 * npm i -g iconv-lite
 * npm i -g silly-datetime
 * npm i -g string-random
 * npm i -g zip-local
 */
var path    = require('path')
var cp      = require('child_process')
var fs      = require('fs')
var iconv   = require('iconv-lite')
var sd      = require('silly-datetime')
var randdir = require('string-random')
var zipper  = require("zip-local")
var util    = require("util")

// 日期
var now = new Date()

// 打包参数
var product = {
    "PublishEnv":       process.env.PublishEnv       || "DEV",
    "PackageName":      process.env.PackageName      || "INSTORE-SERVER-Setup-Win32",
    "VersionMajor":     process.env.MajorVersion     || "0",
    "VersionMinor":     process.env.MinorVersion     || "0",
    "VersionBuild":     process.env.BuildVersion     || "0",
    "VersionRevision":  process.env.RevisionVersion  || "0"
}

// 版本字符串
var productPackVersion = [
    product.VersionMajor,
    product.VersionMinor,
    product.VersionBuild,
    product.VersionRevision
].join('.')

var productPackVersionStr = [
    product.VersionMajor,
    product.VersionMinor,
    product.VersionBuild,
    product.VersionRevision
].join('')

// 目录文件路径
var dirFiles = {
    "msvc2019"          : "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/devenv",
    "build"             : path.resolve(__dirname, '../../Build/Release'),
    "sln"               : path.resolve(__dirname, '../../PosServer.sln'),
    "qt5"               : path.resolve(__dirname, '../../Redist/Qt5.12'),
    "ignore"            : path.resolve(__dirname, '../../Redist/Scripts/ignoreFiles.txt'),
    "nsi"               : path.resolve(__dirname, '../../Redist/Scripts/setup.nsi'),
    "nsis"              : path.resolve(__dirname, '../../Redist/Tools/NSIS/makensis.exe'),
    "signtool"          : path.resolve(__dirname, '../../Redist/Tools/SignTool/signtool.exe'),
    "cert"              : path.resolve(__dirname, '../../Redist/Tools/SignTool/cert_477482561.crt'),
    "output"            : path.resolve(__dirname, '../../Package', sd.format(now, 'YYYY-MM-DD')),
    "temp"              : path.resolve(__dirname, '../../Package/Temp'),
    "bin"               : path.resolve(__dirname, '../../Package/Temp/Bin'),
    "version"           : path.resolve(__dirname, '../../Package/Temp/Bin', productPackVersion),
    "pdb"               : path.resolve(__dirname, '../../Package/Temp/Pdb'),
    "versionFile"       : path.resolve(__dirname, '../../ModuleBase/Version.h'),
};

// 文件名
// 例: INSTORE-SERVER-Setup-Win32-DEV-1.0.0.0-20220328-221508
var productPackName = [
    product.PackageName,
    product.PublishEnv,
    productPackVersion,
    sd.format(now, 'YYYYMMDD-HHmmss')
].join('-')

/** 
 * 生成版本文件 Version.json
{
    "version": "1.0.0.0",
    "code": "1000",
    "date": "2023-12-20",
    "env": "DEV"
}
*/
function createVersionFile() {
    var file = path.join(dirFiles.version, "Version.json")
    var content = util.format("{ \"version\": \"%s\" , \"code\": \"%s\" , \"date\": \"%s\" , \"env\": \"%s\" }",
    productPackVersion, productPackVersionStr, sd.format(now, "YYYY-MM-DD HH:mm:ss"), product.PublishEnv)
    const jsonObject = JSON.parse(content);
    fs.writeFileSync(file, JSON.stringify(jsonObject, null, 4))
    return fs.existsSync(file) ? file : null
}

/** 
 * 生成启动配置文件 Manifest.json
 * { "version" : ["1.0.0.0"] }
 */
 function createManifestFile() {
    var file = path.join(dirFiles.bin, "Manifest.json")
    var content = util.format("{ \"version\": [\"%s\"] }", productPackVersion)
    const jsonObject = JSON.parse(content);
    fs.writeFileSync(file, JSON.stringify(jsonObject, null, 4))
    return fs.existsSync(file) ? file : null
}

/** 字符转码 */
function i18n(data) {
    return iconv.decode(data, 'cp936')
}

/** 递归遍历文件 */
function walkSync(dirname, callback) {
    fs.readdirSync(dirname).forEach(function (name) {
        var filePath = path.join(dirname, name)
        var stat = fs.statSync(filePath)
        if(fs.statSync(filePath).isFile() && typeof callback === "function") {
            callback(filePath, stat)
        } else if(stat.isDirectory()) {
            walkSync(filePath, callback)
        }
    });
}

/** 递归创建文件夹 */
function mkdirsSync(dirname) {
    if(fs.existsSync(dirname)) {
        return true;
    } else {
        if(mkdirsSync(path.dirname(dirname))) {
            fs.mkdirSync(dirname);
            return true;
        }
    }
}

// 递归删除文件夹
function rmdirsSync(dirname) {
    var files = []
    if(fs.existsSync(dirname)) {
        files = fs.readdirSync(dirname)
        files.forEach(function(file, index) {
            var curPath = dirname + "/" + file;
            if(fs.statSync(curPath).isDirectory()) {
                rmdirsSync(curPath);
            } else {
                fs.unlinkSync(curPath);
                console.log(curPath)
            }
        })
        fs.rmdirSync(dirname);
        console.log(dirname)
    }
}

/** 文件签名 */
function signFileSync(filePath) {
    try {
        cp.execFileSync(dirFiles.signtool, ['sign',
        '/csp', 'DigiCert Signing Manager KSP',
        '/kc', 'key_477482561',
        '/f', dirFiles.cert,
        '/tr', 'http://timestamp.digicert.com',
        '/td', 'SHA256',
        '/fd', 'SHA256', filePath])
        return true
    } catch(err) {
        console.log("签名失败：", err.toString())
        return false
    }
}

/** 清理文件 */
function clean() {
    console.log("清理临时文件...")
    rmdirsSync(dirFiles.temp)
    console.log("清理临时文件...OK\n")
    console.log("清理生成文件...")
    rmdirsSync(dirFiles.build)
    console.log("清理生成文件...OK\n")
}

/** 编译源码 */
function compile(sln, callback) {
    console.log("STEP1 ==> 编译源码...")
    var msvc = cp.execFile(dirFiles.msvc2019, [sln,  "/Rebuild", "Release|x86"], { encoding: 'buffer' },
    function(err) {
        if(err) {
            console.error("STEP1 ==> 错误: 编译源码失败", err.toString())
        } else {
            console.log("STEP1 ==> 编译源码...OK ", (new Date() - now) / 1000, "s\n")
        }
        if(typeof callback === 'function') {
            callback(err)
        }
    })
    msvc.stdout.on('data', function (data) {
        console.log(i18n(data))
    })
     
    msvc.stderr.on('data', function (data) {
        console.error(i18n(data))
    });
}

/** 构建工程 */
function build() {
    compile(dirFiles.sln, function(err) {
        if(!err) {
            copyFiles()
            compressFiles()
            signFiles()
            package()
        }
    })
}

/** 拷贝文件 */
function copyFiles() {
    console.log("STEP2 ==> 复制文件...")
    var result = { success: false, msg: null, files: [] }
    if(!fs.existsSync(dirFiles.build)) {
        result.msg = "STEP2 错误: 找不到构建目录"
        console.error(result,msg)
        return result;
    }

    // 创建临时Bin目录
    if(!mkdirsSync(dirFiles.bin)) {
        result.msg = "STEP2 ==> 错误: 创建Bin目录失败"
        console.error(result.msg)
        return result
    }

    // 创建Version目录
    if(!mkdirsSync(dirFiles.version)) {
        result.msg = "STEP2 ==> 错误: 创建Version目录失败"
        console.error(result.msg)
        return result
    }

    // 创建临时PDB目录
    if(!mkdirsSync(dirFiles.pdb)) {
        result.msg = "STEP2 ==> 错误: 创建PDB目录失败"
        console.error(result.msg)
        return result
    }

    // 创建输出目录
    if(!mkdirsSync(dirFiles.output)) {
        result.msg = "STEP2 ==> 错误：创建输出目录失败"
        console.error(result.msg)
        return result
    }

    // 生成版本号文件
    var versionFile = createVersionFile()
    if(!versionFile) {
        result.msg = "STEP2 ==> 错误: 生成版本号文件失败"
        console.error(result.msg)
        return result;
    }
    result.files.push(versionFile)

    

    // 生成启动配置文件
    var manifestFile = createManifestFile()
    if(!manifestFile) {
        result.msg = "STEP2 ==> 错误: 生成启动配置文件失败"
        console.error(result.msg)
        return result;
    }
    result.files.push(manifestFile)
    
    // 复制编译输出文件
    var filters = [".exe", ".dll", ".pdb", ".ini", ".json", ".jpeg", ".jpg", ".png", ".gif", ".svg", ".wav"]
    walkSync(dirFiles.build, function(filePath, stat) {
        var basename = path.basename(filePath)
        var extname = path.extname(filePath)
        if(filters.includes(extname)) {
            if(extname === ".pdb") {
                // PDB文件
                var file = path.join(dirFiles.pdb, basename)
                fs.copyFileSync(filePath, file)
                console.log(filePath, " => ", file)

            } else if(basename === "Launcher.exe") {
                //  Launcher文件
                var file = path.join(dirFiles.bin, basename)
                fs.copyFileSync(filePath, file)
                result.files.push(file)
                console.log(filePath, " => ", file)

            } else {
                // 程序文件
                var relPath = path.relative(dirFiles.build, filePath)
                var relDir = path.dirname(relPath)
                var dir = path.join(dirFiles.version, relDir)
                var file = path.join(dir, basename)
                mkdirsSync(dir)
                fs.copyFileSync(filePath, file)
                result.files.push(file)
                console.log(filePath, " => ", file)
            }
        }
    })

    // 复制Qt5库文件
    walkSync(dirFiles.qt5, function(filePath, stat) {
        var basename = path.basename(filePath)
        var relPath = path.relative(dirFiles.qt5, filePath)
        var relDir = path.dirname(relPath)
        var dir = path.join(dirFiles.version, relDir)
        var file = path.join(dir, basename)
        mkdirsSync(dir)
        fs.copyFileSync(filePath, file)
        result.files.push(file)
        console.log(filePath, " => ", file)
    })
    console.log("STEP2 ==> 复制文件...OK ", (new Date() - now) / 1000, "s\n")

    result.success = true
    result.msg = "成功"
    result.files = result.files
    return result
}

/** 压缩PDB文件 */
function compressFiles() {
    console.log("STEP3 ==> 压缩PDB文件...")
    var zipname = path.join(dirFiles.output, productPackName.concat(".pdb.zip"))
    zipper.sync.zip(dirFiles.pdb).compress().save(zipname);
    console.log("STEP3 ==> 压缩PDB文件...OK ", (new Date() - now) / 1000, "s => ", zipname, "\n")
}

/** 对程序文件签名 */
function signFiles() {
    console.log("STEP4 ==> 数字签名...")
    var ignoreFiles = fs.readFileSync(dirFiles.ignore, { encoding: 'utf8' })
    var filters = [ ".exe", ".dll" ]
    walkSync(dirFiles.bin, function(filePath, stat) {
        var basename = path.basename(filePath)
        var extname = path.extname(filePath)
        if(filters.includes(extname) && !ignoreFiles.includes(basename)) {
            var count = 3
            while (count--) {
                if (signFileSync(filePath)) {
                    console.log(path.basename(filePath), "...OK")
                    break
                } else {
                    console.log("重试签名：", filePath, ",剩余次数：", count)
                }
            }
        }
    })
    console.log("STEP4 ==> 数字签名...OK ", (new Date() - now) / 1000, "s\n")
}

/** 生成安装包 */
function package() {
    console.log("STEP5 ==> 生成安装包...")
    var packageName = productPackName + ".exe"
    var packageFile = path.join(dirFiles.temp, packageName);
    iscc = cp.execFileSync(dirFiles.nsis, ["/DPROGRAM_VERSION=" + productPackVersion, "/DPROGRAM_OUTFILE=" + packageFile, "/DPROGRAM_ROOTDIR=" + dirFiles.bin, dirFiles.nsi])

    // 安装包签名
    var count = 3
    while (count--) {
        if (signFileSync(packageFile)) {
            console.log("安装包签名...OK")
            break
        } else {
            console.log("安装包签名重试, 剩余次数：", count)
        }
    }

    // 安装包备份
    var backupFile = path.join(dirFiles.output, packageName)
    fs.copyFileSync(packageFile, backupFile)
    console.log("STEP5 ==> 生成安装包...OK ", (new Date() - now) / 1000, "s => ", backupFile, "\n")
}

/** 替换Version.h */
function repalce() {
    console.log("STEP0 ==> 替换生成Version.h文件...")
    var file = dirFiles.versionFile
    var data = fs.readFileSync(file, { encoding: 'utf8' })

    var res = data.replace(/#define VERSION_1 \d+/, util.format("#define VERSION_1 %s",product.VersionMajor))
    res = res.replace(/#define VERSION_2 \d+/, util.format("#define VERSION_2 %s",product.VersionMinor))
    res = res.replace(/#define VERSION_3 \d+/, util.format("#define VERSION_3 %s",product.VersionBuild))
    res = res.replace(/#define VERSION_4 \d+/, util.format("#define VERSION_4 %s",product.VersionRevision))
    console.log(res)
    fs.writeFileSync(file, res)
    return fs.existsSync(file) ? file : null
}

/** 运行 */
function run() {
    clean()
    repalce()
    build()
}

run()