const fs = require("fs")
const path = require('path')

let indexHtml = fs.readFileSync("index.html", "utf8");
let mainJs = fs.readFileSync("main.js", "utf8");
let styleCss = fs.readFileSync("style.css", "utf8");

styleCss = '<style>\n' + styleCss + '\n</style>'
mainJs = '<script>\n' + mainJs + '\n</script>'

indexHtml = indexHtml.replace(`<link rel="stylesheet" href="style.css">`, styleCss)
indexHtml = indexHtml.replace(`<script src="main.js"></script>`, mainJs)

const today = new Date();
const date = today.getDate()+'/'+(today.getMonth()+1)+'/'+today.getFullYear() + ' ' + today.getHours() + ':' + today.getMinutes();

const outputData = `/* File generated: ${date} */
const char MAIN_page[] PROGMEM = R"=====(
    ${indexHtml}
)=====";`

const filePath = path.join(__dirname, '../mainPage.h');

fs.writeFile(filePath, outputData, function(error){
    if(error) throw error; // если возникла ошибка
    console.log("Асинхронная запись файла завершена!");
    // console.log("Содержимое файла:");
    // let data = fs.readFileSync("mainPage.h", "utf8");
    // console.log(data);  // выводим считанные данные
});