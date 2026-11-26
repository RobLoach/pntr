const pntrModule = require('..')

async function main() {
    const pntr = await pntrModule()

    console.log("pntr time!")
    const image = pntr._pntr_new_image(200, 200)
    const color = pntr._pntr_new_color(100, 150, 200, 255)

    pntr._pntr_draw_circle(image, 100, 100, 50, color)

    pntr._pntr_save_image(image, "output.png")
}

main()
