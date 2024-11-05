var path    = require('path')
var cp      = require('child_process')
var fs      = require('fs')

/** 递归遍历文件 */
function walkSync(dirname, callback) {
    fs.readdirSync(dirname).forEach(function (name) {
        var filePath = path.join(dirname, name)
        var stat = fs.statSync(filePath)
        if(fs.statSync(filePath).isFile() && typeof callback === "function") {
            callback(filePath, stat)
        } else if (stat.isDirectory()) {
            walkSync(filePath, callback)
        }
    });
}

/** Qt组件 */
function qt5Files() {
    var dllDir = path.join(__dirname, '../../Redist/Qt5.12')
    var filters = [ ".exe", ".dll" ]
    var files = []

    walkSync(dllDir, function(filePath, stat) {
        if(filters.includes(path.extname(filePath))) {
            var filename = path.basename(filePath)
            files.push(filename)
        }
    })
    return files
}

/** 前置机组件 */
function offsiteServerFiles() {
	var dir = path.resolve(__dirname, '../../Redist/offline_server')
    var filters = [ ".exe", ".dll" ]
    var files = []

    walkSync(dir, function(filePath, stat) {
        if(filters.includes(path.extname(filePath))) {
            var filename = path.basename(filePath)
            files.push(filename)
        }
    })
    return files
}

var ignoreFiles = qt5Files() + offsiteServerFiles()
console.log("Ignore Files =>\n", ignoreFiles)
fs.writeFileSync("ignoreFiles.txt", ignoreFiles)