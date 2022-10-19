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
    document.querySelector('#currentIP').value = setup.ip

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