const SerialPort = require("serialport")
const { exec } = require("child_process")

const device = `/dev/tty.usbmodem14101`

// | Note that these property values correspond to the `pokemon` enum
// | in the `multi-rfid.ino` file. Yeah we could just use an array index order
// | instead of numbered properties, but I'd rather do it this way so that
// | the order of the entries into this object don't determine the enum values
// | they correspond to.
const urls = {
    0: "https://en.wikipedia.org/wiki/Bulbasaur",
    1: "https://en.wikipedia.org/wiki/Charmander",
    2: "https://en.wikipedia.org/wiki/Squirtle",
    3: "https://en.wikipedia.org/wiki/Pikachu"
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
        const enumValue = +data

        if (enumValue in urls) {
            const pokemon = urls[enumValue]
            openWikiPage(pokemon)
        } else {
            console.error(
                `Unable to find the pokemon name '${data}' in the url list.`
            )
        }

        setTimeout(() => {
            running = false
        }, 1000)
    }
}

function openWikiPage(url) {
    const command = `open ${url}`
    exec(command, (error, stderr, stdout) => {
        if (error) {
            console.error("Error opening web page")
            console.error(error)
        }
        console.log(`Open page for ${url}`)
    })
}
