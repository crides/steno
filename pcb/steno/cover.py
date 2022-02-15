import cadquery as cq
from cadquery import exporters

thick = 4
top_mount = (40, 40)
bot_mount = (27.5, -10)
lcd_mod_w, lcd_mod_h, lcd_disp_w, lcd_disp_h, lcd_thick = 40, 56.85, 36, 48, 2.75
m2_hole_d, m2_nut_outer_d = 2.1, 5
flash = (6, 11, 3)
flash_loc = -flash[0] - lcd_mod_w / 2, 13.5

flip = lambda p: (-p[0], p[1])
hole = lambda dim, loc: cq.Workplane().moveTo(loc[0], loc[1] - dim[1]).box(*dim, centered=False)
res = (
    cq.importers.importDXF('../Gerber/steno-Edge_Cuts.dxf').wires(">Y").toPending().extrude(thick).intersect(
        cq.importers.importDXF('../Gerber/steno-User_Eco1.dxf', tol=1e-4).wires().toPending().extrude(thick))
    .edges("|Z").fillet(1)
    .edges(">Z").fillet(1)
    .faces(">Z").workplane().pushPoints([top_mount, flip(top_mount), bot_mount, flip(bot_mount)])
        .cboreHole(m2_hole_d, m2_nut_outer_d, thick - 2)
    .moveTo(-32.5, 40)
        .sketch().rect(9.5, 3).push([(0, -2)]).rect(2, 1)
        .clean().finalize().cutThruAll()
        .cut(cq.Workplane().moveTo(-32.5, 40).box(6.5, 7, 1.5))
    .cut(cq.Workplane().moveTo(0, -33).box(lcd_mod_w, lcd_mod_h, lcd_thick, centered=[True, False, False]))
    .moveTo(0, -(lcd_disp_h - (lcd_mod_h - 33 - 1))).rect(lcd_disp_w, lcd_disp_h, centered=[True, False]).cutThruAll()
    .cut(hole(flash, flash_loc))
    .cut(hole((15, 19, 3.5), (-23.5, 48)))      # Holyiot
    .cut(hole((8.5, 5.5, 10), (12.5, 36.5)))    # reset
    .cut(cq.Workplane().workplane(offset=16).moveTo(16.75, 33.75).sphere(15))    # reset cave
    .cut(hole((21, 18.25, 1.5), (-8.75, 45)))   # PMU & passives
    .cut(hole((3.25, 6, 1), (20, -5.5)))        # backlight resistors
    .cut(hole((9.5, 8.5, 3.5), (-4.75, 48)))    # USB
    .cut(hole((9, 4.75, 1.75), (12, 47.25)))    # switch
    .cut(hole((9, 1.5, 10), (12, 48.25)))       # switch nub
    .moveTo(-3.5, 35.25).circle(1).cutThruAll()
    .moveTo(8, 43.25).circle(1).cutThruAll()
)
exporters.export(res, "cover.stl")
cq.Assembly(res, color=cq.Color("gray")).save("cover.step")
