/* File generated: 25/12/2021 8:50 */
const char MAIN_page[] PROGMEM = R"=====(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link rel="manifest" href="manifest.json">

    <meta name="mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="application-name" content="ESP Light">
    <meta name="apple-mobile-web-app-title" content="ESP Light">
    <meta name="theme-color" content="#171717">
    <meta name="msapplication-navbutton-color" content="#171717">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta name="msapplication-starturl" content="/">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <link rel="icon" sizes="512x512" href="favicon.png">
    <link rel="apple-touch-icon" sizes="512x512" href="favicon.png">

    <meta http-equiv="Cache-control" content="public">

    <title>ESP Гирлянда</title>
    <style>
:root {
    --btn-size: 32px;
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    background-color: #222;
    color: #fff;
    font-family: Calibri, sans-serif;
    display: flex;
    flex-direction: column;
    overflow: hidden;
}

button {
    padding: 2px 5px;
}

input[type=text], input[type=password] {
    max-width: 127px;
}

.header {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    background-color: #171717;
    display: flex;
    align-items: center;
    justify-content: center;
    height: 50px;
    font-weight: bold;
    font-size: 1.3em;
    text-shadow: 0 0 10px rgba(255, 170, 0, 1);
}

.page {
    position: absolute;
    left: 0;
    right: 0;
    top: 50px;
    bottom: calc(var(--btn-size) + 10px);
    overflow: auto;
    padding: 10px;
    display: none;
    flex-direction: column;
}

.block_header {
    border-bottom: 1px solid rgba(255, 255, 255, .3);
    margin-bottom: 3px;
}

.block_header h2 {
    padding-left: 5px;
}

.block_content {
    display: flex;
    flex-direction: column;
    align-items: center;
    margin: 10px 0;
}

ul.settings {
    list-style: none;
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
}

li.setting {
    display: flex;
    align-items: center;
    margin: 5px 0;
    width: 100%;
}

li.setting label {
    width: 50%;
    margin-right: 5px;
    text-align: right;
}

li.mode {
    margin: 3px 0;
}

li.mode label {
    text-align: right;
}

input {
    outline: none;
}

.footer {
    position: fixed;
    bottom: 0;
    left: 0;
    right: 0;
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    grid-auto-rows : calc(var(--btn-size) + 20px);
}

.btn {
    text-align: center;
    background-color: #212121;
    border-right: 2px solid rgba(0, 0, 0, .5);
    border-top: 1px solid rgba(0, 0, 0, .5);
    background-size: var(--btn-size);
    background-repeat: no-repeat;
    background-position: center center;
}

.btn:last-child {
    border-right: none;
}

.btn.settings {
    background-image: url(data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiA/PjxzdmcgZGF0YS1uYW1lPSJMYXllciAxIiBoZWlnaHQ9IjIwMCIgaWQ9IkxheWVyXzEiIHZpZXdCb3g9IjAgMCAyMDAgMjAwIiB3aWR0aD0iMjAwIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPjx0aXRsZS8+PHBhdGggZD0iTTM2LjUsNzVDNDEsODkuNSw1NCwxMDAsNzAsMTAwczI5LTEwLjUsMzMuNS0yNUgxNzBhMTAsMTAsMCwwLDAsMC0yMEgxMDMuNUM5OSw0MC41LDg2LDMwLDcwLDMwUzQxLDQwLjUsMzYuNSw1NUgyNWExMCwxMCwwLDAsMCwwLDIwWk03MCw1MEExNSwxNSwwLDEsMSw1NSw2NSwxNC43MywxNC43MywwLDAsMSw3MCw1MFptMTA1LDc1SDE2My41QzE1OSwxMTAuNSwxNDYsMTAwLDEzMCwxMDBzLTI5LDEwLjUtMzMuNSwyNUgzMGExMCwxMCwwLDAsMCwwLDIwSDk2LjVjNC41LDE0LjUsMTcuNSwyNSwzMy41LDI1czI5LTEwLjUsMzMuNS0yNUgxNzVhMTAsMTAsMCwwLDAsMC0yMFptLTQ1LDI1YTE1LDE1LDAsMSwxLDE1LTE1QTE0LjczLDE0LjczLDAsMCwxLDEzMCwxNTBaIiBmaWxsPSIjZmZmZmZmIi8+PC9zdmc+Cg==);
}

.btn.modes {
    background-image: url(data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiA/Pjxzdmcgdmlld0JveD0iMCAwIDI0IDI0IiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPjxnPjxwYXRoIGQ9Ik0wIDBoMjR2MjRIMHoiIGZpbGw9Im5vbmUiLz48cGF0aCBkPSJNMjAgMjJINGExIDEgMCAwIDEtMS0xVjNhMSAxIDAgMCAxIDEtMWgxNmExIDEgMCAwIDEgMSAxdjE4YTEgMSAwIDAgMS0xIDF6TTggN3YyaDhWN0g4em0wIDR2Mmg4di0ySDh6bTAgNHYyaDV2LTJIOHoiIGZpbGw9IiNmZmZmZmYiLz48L2c+PC9zdmc+Cg==);
}

.btn.system {
    background-image: url(data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiA/PjxzdmcgaGVpZ2h0PSI0OCIgdmlld0JveD0iMCAwIDQ4IDQ4IiB3aWR0aD0iNDgiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PHBhdGggZD0iTTAgMGg0OHY0OEgweiIgZmlsbD0ibm9uZSIvPjxwYXRoIGQ9Ik0zOC44NiAyNS45NWMuMDgtLjY0LjE0LTEuMjkuMTQtMS45NXMtLjA2LTEuMzEtLjE0LTEuOTVsNC4yMy0zLjMxYy4zOC0uMy40OS0uODQuMjQtMS4yOGwtNC02LjkzYy0uMjUtLjQzLS43Ny0uNjEtMS4yMi0uNDNsLTQuOTggMi4wMWMtMS4wMy0uNzktMi4xNi0xLjQ2LTMuMzgtMS45N0wyOSA0Ljg0Yy0uMDktLjQ3LS41LS44NC0xLS44NGgtOGMtLjUgMC0uOTEuMzctLjk5Ljg0bC0uNzUgNS4zYy0xLjIyLjUxLTIuMzUgMS4xNy0zLjM4IDEuOTdMOS45IDEwLjFjLS40NS0uMTctLjk3IDAtMS4yMi40M2wtNCA2LjkzYy0uMjUuNDMtLjE0Ljk3LjI0IDEuMjhsNC4yMiAzLjMxQzkuMDYgMjIuNjkgOSAyMy4zNCA5IDI0cy4wNiAxLjMxLjE0IDEuOTVsLTQuMjIgMy4zMWMtLjM4LjMtLjQ5Ljg0LS4yNCAxLjI4bDQgNi45M2MuMjUuNDMuNzcuNjEgMS4yMi40M2w0Ljk4LTIuMDFjMS4wMy43OSAyLjE2IDEuNDYgMy4zOCAxLjk3bC43NSA1LjNjLjA4LjQ3LjQ5Ljg0Ljk5Ljg0aDhjLjUgMCAuOTEtLjM3Ljk5LS44NGwuNzUtNS4zYzEuMjItLjUxIDIuMzUtMS4xNyAzLjM4LTEuOTdsNC45OCAyLjAxYy40NS4xNy45NyAwIDEuMjItLjQzbDQtNi45M2MuMjUtLjQzLjE0LS45Ny0uMjQtMS4yOGwtNC4yMi0zLjMxek0yNCAzMWMtMy44NyAwLTctMy4xMy03LTdzMy4xMy03IDctNyA3IDMuMTMgNyA3LTMuMTMgNy03IDd6IiBmaWxsPSIjZmZmZmZmIi8+PC9zdmc+Cg==);
}

.btn.active {
    box-shadow: inset 3px 3px 10px rgba(0, 0, 0, .5);
    background-color: #171717;
}
</style>
</head>
<body>
    <div class="header">ESP cLight</div>
    <div class="page">
        <div class="block_header">
            <h2>Настройки</h2>
        </div>
        <div class="block_content">
            <ul class="settings">
                <li class="mode">
                    <button id="powerOnOff">ВЫКЛЮЧИТЬ</button>
                </li>
                <li class="setting">
                    <label for="colorSelector">Выбор цвета:</label>
                    <input type="color" name="colorSelector" id="colorSelector">
                </li>
                <li class="setting">
                    <label for="brightness">Яркость:</label>
                    <input type="range" id="brightness" name="brightness" min="0" max="255" value="255">
                </li>
                <li class="setting">
                    <label for="speed">Скорость режима:</label>
                    <input type="range" id="speed" name="speed" min="1" max="100" value="90">
                </li>
                <li class="setting">
                    <label for="gliterOnOff">Вспышки:</label>
                    <input type="checkbox" name="gliterOnOff" id="gliterOnOff">
                </li>
                <li class="setting">
                    <label for="candleOnOff">Свечи:</label>
                    <input type="checkbox" name="candleOnOff" id="candleOnOff">
                </li>
            </ul>
        </div>
    </div>
    <div class="page">
        <div class="block_header">
            <h2>Режимы</h2>
        </div>
        <div class="block_content">
            <ul class="settings" id="modes">
                <li class="mode">
                    <button id="autochange">Автопереключение режимов</button>
                </li>
            </ul>
        </div>
    </div>
    <div class="page">
        <div class="block_header">
            <h2>Система</h2>
        </div>
        <div class="block_content">
            <ul class="settings">
                <li class="setting">
                    <label for="ssid">SSID:</label>
                    <input type="text" name="ssid" id="ssid">
                </li>
                <li class="setting">
                    <label for="pass">Пароль:</label>
                    <input type="password" name="pass" id="pass">
                </li>
                <li class="setting">
                    <label for="countLed">Количество LED:</label>
                    <input type="text" name="countLed" id="countLed">
                </li>
                <li class="mode">
                    <button id="saveSystem">Сохранить</button>
                    <button id="resetSystem">Перезагрузить</button>
                </li>
            </ul>
        </div>
    </div>
    <div class="footer">
        <div class="btn settings"></div>
        <div class="btn modes"></div>
        <div class="btn system"></div>
    </div>

    <script>
const MAX_SPEED = 100
const MIN_SPEED = 1

const $colorSelector = document.querySelector('input#colorSelector')
const $brightness = document.querySelector('input#brightness')
const $effectSpeed = document.querySelector('input#speed')
const $buttonSaveSystem = document.querySelector('button#saveSystem')
const $buttonPowerOnOff = document.querySelector('button#powerOnOff')
const $gliterOnOff = document.querySelector('input#gliterOnOff')
const $candleOnOff = document.querySelector('input#candleOnOff')
const $autochange = document.querySelector('button#autochange')
const $btnResetESP = document.querySelector('button#resetSystem')

const $btns = document.querySelectorAll('.btn')
const $pages = document.querySelectorAll('.page')

$btns.forEach((btn, i) => {
    btn.setAttribute("data-id", i)

    btn.addEventListener('click', handleClickBtn)
})

$effectSpeed.setAttribute("max", MAX_SPEED - 1)
$effectSpeed.setAttribute("min", MIN_SPEED)
$effectSpeed.value = MAX_SPEED - 10

function componentToHex(c) {
    var hex = c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
}

function rgbToHex(r, g, b) {
    return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

$colorSelector.addEventListener('change', (event) => {
    const $modes = document.querySelectorAll('.mode input')

    $modes.forEach(modeItem => {
        modeItem.checked = false
    })

    setColor(hexToRgb(event.target.value))
})

function objIsEmpty(obj) {
    for (let key in obj) {
        return false;
    }
    return true;
}

$effectSpeed.addEventListener('input', handleChangeSpeed)
$brightness.addEventListener('input', handleChangeBrightness)
$buttonSaveSystem.addEventListener('click', handleClickSaveSystem)
$buttonPowerOnOff.addEventListener('click', handlePowerOnOff)
$gliterOnOff.addEventListener('change', handleGlitter)
$candleOnOff.addEventListener('change', handleCandle)
$autochange.addEventListener('click', handleAutochange)
$btnResetESP.addEventListener('click', handleReset)

async function query(link = "/", params = {}, type = "GET") {
    let reqLink = link

    if (!objIsEmpty(params)) {
        reqLink += "?"
        for (let key in params) {
            reqLink += key + "=" + params[key] + "&"
        }
        reqLink = reqLink.slice(0, -1)
    }

    console.log("QUERY: " + reqLink);

    try {
        const response = await fetch(reqLink, { method: type })
        const res = await response.json()
        console.log("RESPONSE: ");
        console.log(res);
        return res
    } catch (error) {
        console.log(error);
    }
}

async function handleReset(event) {
    const res = await query('/reset')
}

async function handleGlitter(event) {
    const result = await query('/setGlitter')

    if (parseInt(result.glitter)) {
        event.target.checked = true;
        return;
    } 
    event.target.checked = false;
}

async function handleCandle(event) {
    const result = await query('/setCandle')

    if (parseInt(result.candle)) {
        event.target.checked = true;
        return;
    } 
    event.target.checked = false;
}

async function handleAutochange(event) {
    const result = await query('/setAutochange')
}

async function handlePowerOnOff(event) {
    const result = await query('/powerOnOff')

    if (parseInt(result.power)) {
        event.target.innerHTML = "ВЫКЛЮЧИТЬ";
        return;
    } 
    event.target.innerHTML = "ВКЛЮЧИТЬ";
}

async function handleClickSaveSystem(event) {
    setup.ssid = document.querySelector('#ssid').value
    setup.pass = document.querySelector('#pass').value
    setup.leds = document.querySelector('#countLed').value

    const result = await query('/saveSystem', {
        "ssid": setup.ssid,
        "pass": setup.pass,
        "leds": setup.leds
    })
}

async function handleChangeSpeed(event) {
    const result = await query('/setSpeed', {
        speed: MAX_SPEED - event.target.value
    })
}

async function handleChangeBrightness(event) {
    const result = await query('/setBrightness', {
        brightness: event.target.value
    })
}

async function setColor(color) {
    const result = await query('/setColor', color)
}

async function handleClickMode(event) {
    const result = await query('/setMode', {
        mode: event.target.value
    })

    $effectSpeed.value = MAX_SPEED - setup.modes[event.target.value].effectDelay
}

function installModes() {
    const $modes = document.querySelector('#modes')

    setup.modes.forEach((mode, i) => {
        const html = `
            <li class="mode">
                <input type="radio" id="${mode.name + i}" name="mode" value="${i}">
                <label for="${mode.name + i}">${i + 1}. ${mode.name}</label>
            </li>
        `
        $modes.insertAdjacentHTML('beforeend', html)
    })

    const $modesInputs = $modes.querySelectorAll('input');
    $modesInputs.forEach(modeInput => {
        modeInput.addEventListener('change', handleClickMode)
    })
}

function installFields() {
    document.querySelector('#ssid').value = setup.ssid
    document.querySelector('#pass').value = setup.pass
    document.querySelector('#countLed').value = setup.leds

    if (parseInt(setup.power)) {
        $buttonPowerOnOff.innerHTML = "ВЫКЛЮЧИТЬ"
    } else {
        $buttonPowerOnOff.innerHTML = "ВКЛЮЧИТЬ"
    }

    $brightness.value = setup.brightness
    $effectSpeed.value = MAX_SPEED - setup.speed

    if (parseInt(setup.glitter)) {
        $gliterOnOff.checked = true
    }

    if (parseInt(setup.candle)) {
        $candleOnOff.checked = true
    }
}

var setup

(async () => {
    try {
        setup = await query('/getSetup')
        installModes()
        installFields()
    } catch (e) {
        console.log(e);
    }
})()

function activatePage(num) {
    $pages.forEach(page => {
        page.style.display = "none"
    })

    $btns.forEach(btn => {
        btn.classList.remove("active")
    })

    $btns[num].classList.add("active")
    $pages[num].style.display = "flex"
}

function handleClickBtn(event) {
    const id = event.target.dataset.id
    activatePage(id)
}

activatePage(1)
</script>
</body>
</html>
)=====";