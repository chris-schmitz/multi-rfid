const SerialPort = require("serialport")
const { exec } = require("child_process")

const device = `/dev/tty.usbmodem14101`
const urls = {
    bulbasaur: "https://en.wikipedia.org/wiki/Bulbasaur",
    charmandar: "https://en.wikipedia.org/wiki/Charmander",
    squirtle: "https://en.wikipedia.org/wiki/Squirtle",
    pikachu: "https://en.wikipedia.org/wiki/Pikachu"
}

const port = new SerialPort(device, {
    baudRate: 9600
})
const ReadlineParser = require("@serialport/parser-readline")
const parser = new ReadlineParser()

port.pipe(parser)

parser.on("data", handleTagRead)

port.on("readable", () => {
    console.log(`got more data:`, port.read())
})

let running = false

function handleTagRead(data) {
    if (!running) {
        running = true
        console.log("got new data")
        console.log(data)
        openWikiPage("bulbasaur")

        setTimeout(() => {
            running = false
        }, 1000)
    }
}

function openWikiPage(pokemon) {
    const command = `open ${urls.bulbasaur}`
    exec(command, (error, stderr, stdout) => {
        if (error) {
            console.error("Error opening web page")
            console.error(error)
        }
        console.log(`Open page for ${pokemon}`)
    })
}
