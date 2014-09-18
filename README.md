mtetris
=======

I acquired this code sometime in the early 1990s. I believe it originated with a DEC engineer in Japan (I was working for DEC at the time), and was originally called "dwtetris"; it seems to have been written for DECWindows. My only "contributions" consist of some minor changes to make it compile on modern compilers, and some formatting for readability; otherwise it's relatively unimproved and unmodified since I obtained it.

The game works and plays reasonably well (tested on OS X and Ubuntu). Aside from providing a little free fun, I've made this code available to provide an insight into how UI coding was done in X11/Motif a couple decades (or more) ago (particularly if one didn't have access to, or use, an interactive GUI design tool). This involved explicit construction of all widgets, explicit specification of all sizes and constraints, and association of callbacks with various events. Not very scalable, as this all requires a lot of typing, and it's rather error-prone and inflexible...
