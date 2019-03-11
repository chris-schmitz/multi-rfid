const SerialPort = require("serialport")
const { exec } = require("child_process")
const { cyan, red, green, magenta } = require("chalk")

// | Note that these property values correspond to the `pokemon` enum
// | in the `multi-rfid.ino` file. Yeah we could just use an array index order
// | instead of numbered properties, but I'd rather do it this way so that
// | the order of the entries into this object don't determine the enum values
// | they correspond to.
const urls = {
    0: "https://bulbapedia.bulbagarden.net/wiki/Bulbasaur",
    1: "https://bulbapedia.bulbagarden.net/wiki/Charmander",
    2: "https://bulbapedia.bulbagarden.net/wiki/Squirtle",
    3: "https://bulbapedia.bulbagarden.net/wiki/Pikachu"
}

let running = false

run()
    .then(() => {
        console.log("Serial server is running")
    })
    .catch(error => {
        console.log(red("There was an error: "))
        console.error(error.message)
        console.error(error.stack)
    })

async function run() {
    try {
        const device = await determineUsbPath()
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
    } catch (error) {
        throw error
    }
}

function handleTagRead(data) {
    if (!running) {
        running = true
        const enumValue = +data

        if (isNaN(enumValue)) {
            throw new Error(
                "\n\nWe're getting non-numeric data from the microcontroller.\nMake sure the sketch's 'VERBOSE_SERIAL' constant is set to 'false'.\n\n"
            )
        }

        if (enumValue in urls) {
            const pokemon = urls[enumValue]
            openWikiPage(pokemon)
        } else {
            console.log(
                magenta(
                    `Unable to find the pokemon name "${enumValue}" in the url list.`
                )
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
            console.error(red("Error opening web page"))
            console.error(red(error))
        }
        console.log(green(`Open page for ${url}.`))
    })
}

function determineUsbPath() {
    return new Promise((resolve, reject) => {
        const command = "ls /dev/ | grep tty.usb"

        exec(command, (error, stdout, stderr) => {
            if (error) {
                return reject(error)
            }

            resolve(`/dev/${stdout}`.trim())
        })
    })
}
