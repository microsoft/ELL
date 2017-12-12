---
layout: default
title: Active cooling your Raspberry Pi 3
permalink: /tutorials/Active-cooling-your-Raspberry-Pi-3/
---

# Active cooling your Raspberry Pi 3

*by Ofer Dekel and Spencer Persaud*


The Raspberry Pi 3 tends to overheat when pushed to its limits. When the processor's internal temperature approaches 85 degrees Celsius, it protects itself by clocking down or shutting down completely, and the performance of our AI models takes a hit.

![Idle RPi3 heatmap](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Pi-3-IR.jpg)

These infrared images show a Pi running idle (left) and after a few minutes of running a compute-intensive AI model (right). Notice that the main processor heats up much more than any of the other components on the board. Some cooling kits for the Raspberry Pi include heatsinks for the other components, but these infrared images suggest that we should really focus on cooling the processor. In this tutorial, we will build a simple and effective active cooling solution for the Raspberry Pi 3.

#### Materials:

* [Adafruit Aluminum Heat Sink for Raspberry Pi 3 - 15 x 15 x 15mm](https://www.adafruit.com/product/3082) (comes with a thermally conductive sticker), or equivalent.
* [Adafruit Miniature 5V Cooling Fan for Raspberry Pi](https://www.adafruit.com/product/3368) (comes with mouting screws and nuts), or equivalent 5V 0.2A DC brushless fan, 30mm x 30mm, with mounting holes spaced 24mm apart.
* The secret ingredient: our *Pi 3 Fan Mount*. You need to 3D print [this part](/ELL/gallery/Raspberry-Pi-3-Fan-Mount).
* Two M2.5 x 12 pan head machine screws and nuts, to attach the fan mount to the circuit board.

![Pi Active Cooling Materials](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Pi-Active-Cooling-Materials.jpg)

## Assembly:

**Step 1:** Attach the fan to the 3D printed fan mount using the screws and nuts provided with the fan. Make sure that the fan is oriented such that it blows air towards the mount (when installed, it will blow air on the heatsink, rather than sucking air from the heatsink).

![Attach-Fan-to-Mount](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Attach-Fan-to-Mount.jpg)

**Step 2:** Attach the fan mount to the Pi circuit board using the two M2.5 x 12 machine screws and nuts.

**Step 3:** Stick the heat sink on the processor. **Make sure to align the heat sink fins with the air flow from the fan**.

**Step 4:** Plug the fan into the 5V and ground pins on the Pi.

![Pi with Fan](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Pi-with-Fan.jpg)

## Mounting on the Raspberry Pi 7" Touchscreen Display

Most other active cooling solutions for the Raspberry Pi come in the form of an active cooling enclosure or case. A variety of different active cooling cases can be purchased online and some of them work well. However, enclosing the Pi in a case isn't always desirable. For example, in many of our own projects, we like to mount Raspberry Pis onto the Raspberry Pi 7" Touchscreen Display. Therefore, we specifically designed our active cooling solution to be compatible with the 7" Touchscreen Display. The only difference in the assembly instructions is that we don't need nuts for the M2.5 x 12 screws. Instead, they screw directly into the M2.5 standoffs that come with the 7" Touchscreen Display.

![Pi with Fan on Display](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Pi-with-Fan-on-Display.jpg)

## Printing without standoffs

You may have noticed that the *Pi 3 Fan Mount* comes in two versions, with or without standoffs. If your 3D printer can print support material, then print the version with standoffs. On the other hand, if your printer only supports a single filament, you may have more luck printing the version without standoffs. In that case, you will need to add nylon spacers or standoffs to lift the plastic mount off of the Pi circuit board.

![Two Mount Designs](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Two-Mount-Designs.jpg)

## Discussion

We've seen quite a few recommendations to cool the Pi using only a heatsink, without a fan. In our experience, using a heatsink without a fan is insufficient. To demonstrate this, we stress tested the Pi by running all four cores at 100% and measured the processor temperature as it heats up.  We repeated the experiment with four different configurations:

* `none` - no cooling at all, just a Raspberry Pi as-is
* `heat sink` - heat sink on the processor, fan turned off
* `fan` - fan mounted on the Rapsberry Pi using our fan mount, blowing air on the bare processor, no heat sink
* `both` - our full cooling solution, as described above, with both fan and heat sink

Here are the results we observed:

<div id='plot'></div>
<script>
var spec = {
  "$schema": "https://vega.github.io/schema/vega-lite/v2.json",
  "description": "Pi temperature during the stress test, under four conditions",
  "width": 500, "height": 300,
  "selection": { "grid": { "type": "interval", "bind": "scales" } },
  "data": { "url": "/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/pi-heating-data-long.tsv" },
  "mark": "line",
  "encoding": {
    "x": { "field": "x", "type": "quantitative", "axis": {"title":""} },
    "y": { "field": "value", "type": "quantitative", "axis": {"title": "Processor Temperature ºC"} },
    "color": { "field": "variable", "type": "nominal", "legend":{"title":"Conditions"} }
  }
}
vegaEmbed("#plot", spec, {actions:false})
</script>

The x-axis represents time in seconds and the y-axis represents the processor temperature in Celsius. The measurement starts with the idle processor temperature, and the stress test begins after 20 seconds. Click and drag to pan around. Zoom in and out with your mouse's scrollwheel.

The `none` configuration quickly overheats. Within a few minutes, the processor temperature hits 85 degrees, the temperature at which the processor starts protecting itself by throttling down its frequency. The passive cooling configuration, `heat sink`, isn't much better. At first, the heat sink absorbs some of the heat, causing the processor temperature to rise more slowly. However, the heat sink struggles to dissipate that heat and the processor temperature gradually climbs into the 70's. The passive heat sink prevented the processor from reaching the critical temperature of 85 degrees, but came too close for comfort. Processor temperature depends on many factors, such as ambient temperature, processor load, and processor frequency. Moreover, different Raspberry Pi units behave differently. The experiment was conducted in an air conditioned office (room temperature was about 26 degrees Celsius) and we can imagine getting into trouble under different circumstances.

We were surprised to see the effectiveness of the fan in the `fan` configuration, where the processor temperature quickly stabilizes around 63 degrees. The clear winner is the `both` configuration, where the combination of fan and heat sink works like a champion, and the processor temperature remains below 50 degrees.

Try to repeat the experiment with different AI workloads and different cooling configurations. To measure processor temperature, use the command `watch /opt/vc/bin/vcgencmd measure_temp`.

Now that you know how to cool your Raspberry Pi, you can build a Raspberry Pi cluster.

![Cluster](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/Cluster.jpg)

