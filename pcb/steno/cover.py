from solidff import *
import solid

top_mount = (40, 40)
bot_mount = (27.5, -10)
lcd_mod_w, lcd_mod_h, lcd_disp_w, lcd_disp_h, lcd_thick = 40, 56.85, 36, 48, 2.75
m2_hole_d, m2_nut_outer_d = 2.1, 5
flash = (6, 11, 3)
flash_loc = -flash[0] - lcd_mod_w / 2, 13.5

flip = lambda p: (-p[0], p[1])
topleft = lambda dim, loc: q(*dim).y(-dim[1]).t(*loc)
mount_hole = lambda p: (cy(d=m2_hole_d, h=10) + cy(d=m2_nut_outer_d, h=10).z(2)).t(*p)
jlink_hole = ((s(9.5, 3, True) + s(2, 1, 'x').y(-1-3/2)).e(10) + q(6.5, 7, 1.5, True)).t(-32.5, 40)
screen_mod = s(lcd_mod_w, lcd_mod_h, 'x').y(-33).e(lcd_thick)
screen_win = s(lcd_disp_w, lcd_disp_h, 'x').y(-(lcd_disp_h - (lcd_mod_h - 33 - 1))).e(10)

switches = solid.import_('../Gerber/steno-User_Eco1.dxf').add_param('$fn', 60)
edge = solid.import_('../Gerber/steno-Edge_Cuts.dxf').add_param('$fn', 60)
((edge * switches + s(39, 11).t(-19.25, -33.25)).e(4)
 - mount_hole(top_mount) - mount_hole(flip(top_mount))
 - mount_hole(bot_mount) - mount_hole(flip(bot_mount))
 - jlink_hole
 - screen_mod - screen_win
 - topleft(flash, flash_loc)
 - topleft((15, 19, 3.5), (-23.5, 48))      # Holyiot
 - topleft((8.5, 5.5, 10), (12.5, 36.5))    # reset
 - b(d=30).z(16).t(16.75, 33.75)            # reset cave
 - cy(d=2, h=10).t(-3.5, 35.25) - cy(d=2, h=10).t(8, 43.25) # LEDs
 - topleft((21, 18.25, 1.5), (-8.75, 45)) # PMU & passives
 - topleft((3.25, 6, 1), (20, -5.5))        # backlight resistors
 - topleft((9.5, 8.5, 3.5), (-4.75, 48))    # USB
 - topleft((9, 4.75, 1.75), (12, 47.25))    # switch
 - topleft((9, 1.5, 10), (12, 48.25))       # switch nub
 ).dump_this()
