import math
from typing import Tuple
import cadquery as cq
from cadquery import exporters
import pcbnew, re
from pcbnew import Iu2Millimeter as iu2mm

board = pcbnew.LoadBoard("steno.kicad_pcb")
center = board.GetDesignSettings().GetAuxOrigin()
def trans_point(p: pcbnew.wxPoint) -> Tuple[float, float]:
    off = p - center
    return iu2mm(off.x), -iu2mm(off.y)

thick = 4
mount_holes = [trans_point(p.GetPosition()) for p in board.FindFootprintByReference("H2").Pads()]
lcd_mod_w, lcd_mod_h, lcd_disp_w, lcd_disp_h, lcd_thick = 40, 56.85, 36, 48, 2.75
m2_hole_d, m2_nut_outer_d = 2.1, 5
flash = (6, 11, 3)
flash_loc = -flash[0] - lcd_mod_w / 2, 13.5

back_thick = 4
pin_depth, pin_clear = 2, 1

flip = lambda p: (-p[0], p[1])
hole = lambda dim, loc: cq.Workplane().moveTo(loc[0], loc[1] - dim[1]).box(*dim, centered=False)
edge = cq.importers.importDXF('../Gerber/steno-Edge_Cuts.dxf').wires(">Y").toPending()
def front():
    return (edge.extrude(thick).intersect(
            cq.importers.importDXF('../Gerber/steno-User_Eco1.dxf', tol=1e-3).wires().toPending().extrude(thick))
        .edges("|Z").fillet(1)
        .edges(">Z").fillet(1)
        .faces(">Z").workplane().pushPoints(mount_holes)
            .cboreHole(m2_hole_d, m2_nut_outer_d, thick - 2)
        .moveTo(-32.5, 40)
            .sketch().rect(9.5, 3).push([(0, -2)]).rect(2, 1)
            .clean().finalize().cutThruAll()
            .cut(cq.Workplane().moveTo(-32.5, 40).box(6.5, 7, 1.5))
        .cut(cq.Workplane().moveTo(0, -33).box(lcd_mod_w, lcd_mod_h, lcd_thick, centered=(True, False, False)))
        .moveTo(0, -(lcd_disp_h - (lcd_mod_h - 33 - 1))).rect(lcd_disp_w, lcd_disp_h, centered=(True, False)).cutThruAll()
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

def back():
    back = (
        edge.extrude(-back_thick).faces(">Z").workplane()
    )
    bat_holder_w, bat_holder_thick = 60, 2
    for fp in board.GetFootprints():
        if re.match(r"K\d+", fp.GetReference()):
            for p in fp.Pads():
                if p.GetShape() == pcbnew.PAD_SHAPE_CIRCLE:
                    back = back.moveTo(*trans_point(p.GetPosition())).hole(iu2mm(p.GetSizeX()) + pin_clear, pin_depth, clean=False)
    back = back.clean()
    right_edge = back.faces("%PLANE and #Z").faces(cq.selectors.DirectionSelector(cq.Vector(1, 0, 0), tolerance=3.14/6)).faces(cq.selectors.AreaNthSelector(0, directionMax=False, tolerance=10))
    bat_holder = (right_edge.workplane(offset=-bat_holder_w).box(100, 100, 100, centered=(True, True, False), combine=False)
                  .intersect(back.faces("<Z").wires().toPending().extrude(-7.7, combine=False))
                  .edges("|Z").edges("<X or <Y").fillet(3)
                  .faces(">Z").shell(-2)
                  - right_edge.workplane(offset=-bat_holder_w + bat_holder_thick).transformed((0, 0, 180)).box(100, 4 + 7.7 - 2, bat_holder_w - 2 * bat_holder_thick, centered=False, combine=False))
    back = ((back + bat_holder + bat_holder.mirror("YZ"))
            .edges("<Z").fillet(1)
            .faces(">Z[2]").workplane().moveTo(0, 0).transformed((0, 0, 180))
            .pushPoints(mount_holes).cboreHole(m2_hole_d, m2_nut_outer_d, thick - 2)
            .circle(9/2 + 5).circle(9/2).extrude(7.7))

# exporters.export(back, "cover-back.stl")
# cq.Assembly(front, color=cq.Color("gray")).save("cover-front.step")
cq.Assembly(back(), color=cq.Color("gray")).save("cover-back.step")
