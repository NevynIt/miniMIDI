todo:
- implement custom stdio driver that somehow knows which channel is active at the moment
  - make the hw uart and usb uart go on the same core, so that a ptr can be changed when using the cli from them, so that answers go in the same place the request came from
  - better, check which core you are! core 0 prints, core 1 queues, core 0 prints all the queue in the preferred debug line once every cycle - requires defining a preferred line
- rbj filter, params and refresh filter every x samples
- add a wrapper decorator for wrapping any wave and add asm support
- wave specific buffers ?
- test division in fpm.h