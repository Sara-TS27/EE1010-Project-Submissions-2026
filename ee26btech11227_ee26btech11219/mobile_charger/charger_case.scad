// ============================================================
// USB mobile charger enclosure — v5
// Transformer bolted to the floor FROM UNDERNEATH — screws go in through
// the bottom exterior, up through the floor, into the transformer's tabs.
// Retaining wedges brace it front and back (measured, not guessed).
// Perfboard floats above it on standoffs, screwed from above.
// USB-A cutout is on the FRONT wall, 6mm from the board's edge.
// Ventilation is over the transformer (right wall + dormant lid grid).
// Lid is currently NOT rendered (see bottom) — base_tray() only, so you
// can see the open interior. The lid() module still exists, ready to
// bring back later.
// ============================================================

// ---------- Perfboard (drives the case footprint) ----------
board_w        = 50;   // perfboard width
board_l        = 70;   // perfboard length
board_t        = 1.6;  // perfboard thickness
// mounting hole spacing — measured directly, near-edge (closer ends) gap
// converted to center-to-center by adding back the hole's own diameter
board_hole_dia  = 3.5;   // the board's own hole diameter (not modeled directly)
hole_gap_x      = 41;    // along the 5cm edge — closer-ends gap
hole_gap_y      = 61.4;  // along the 7cm edge — closer-ends gap
hole_spacing_x  = hole_gap_x + board_hole_dia;  // = 44.5mm center-to-center
hole_spacing_y  = hole_gap_y + board_hole_dia;  // = 64.9mm center-to-center
board_hole_inset_x = (board_w - hole_spacing_x) / 2;  // = 2.75mm from each side
board_hole_inset_y = (board_l - hole_spacing_y) / 2;  // = 2.55mm from each side

// clearance between board edge and inner wall
board_margin   = 5;

// ---------- Walls ----------
wall     = 2;
floor_t  = 2;
lid_t    = 2;

// ---------- Transformer ----------
xfmr_w = 30;  // transformer body width (x) — confirmed by measurement

// ---------- Screw bosses for the transformer ----------
boss_dia    = 12;   // widened to comfortably contain the elliptical hole below
boss_height = 6;

// transformer tab holes are elongated slots, long axis pointing toward
// each other (along the spacing direction)
tab_hole_a = 4;   // mm — width (short axis)
tab_hole_b = 8;   // mm — length (long axis, along the spacing direction)

// measured: gap between the two slots' NEAR (curved) ends, not center-to-center
tab_closer_ends_gap = 55;
xfmr_tab_spacing = tab_closer_ends_gap + tab_hole_b; // = 63mm, center-to-center

// ---------- Outer footprint (derived — whichever part needs more room) ----------
board_driven_w = board_w + 2*board_margin + 2*wall;
xfmr_driven_w  = xfmr_tab_spacing + boss_dia + 2*board_margin + 2*wall;
outer_w = max(board_driven_w, xfmr_driven_w);
outer_l = board_l + 2*board_margin + 2*wall;

// tab holes centered on the footprint, sitting on the floor
tab_y      = outer_l / 2;
tab_x1     = outer_w/2 - xfmr_tab_spacing/2;
tab_x2     = outer_w/2 + xfmr_tab_spacing/2;

// ---------- Transformer retaining wedges (right-angle ramps) ----------
// PLACEHOLDER dims — a doorstop-shaped ramp against the transformer's front
// and back faces only: one flat face flush against the transformer body
// (full height), the other flat face flush along the case floor, sloped
// face between them. Runs the full width of each face for continuous
// support. Left/right sides are left open (e.g. for wiring access).
wedge_run       = 16;   // how far the ramp's floor leg extends outward
wedge_clearance = 0.3;  // snug — the vertical face needs a tight fit
wedge_height    = 40;   // fixed per spec

xfmr_x0 = outer_w/2 - xfmr_w/2 - wedge_clearance;
xfmr_x1 = outer_w/2 + xfmr_w/2 + wedge_clearance;

// distance between the two wedges' contact planes — set directly per spec,
// not derived from xfmr_l, since this is a fixed, known measurement
wedge_plane_gap = 47;
xfmr_y0 = outer_l/2 - wedge_plane_gap/2;
xfmr_y1 = outer_l/2 + wedge_plane_gap/2;

// ---------- Perfboard standoffs ----------
standoff_dia       = 6;
// Pilot hole for the screw to self-tap INTO — this must be smaller than
// the screw itself, not the same as the perfboard's 3.5mm clearance hole.
// A screw that passes through a 3.5mm board hole is typically ~3mm
// (M3-class); ~2.5mm is the standard undersized pilot for that screw
// self-tapping into PLA/ABS.
standoff_pilot_dia = 2.5;
component_clear    = 25;  // headroom above board for heatsink/7805 stack

// fixed per spec — not derived from the transformer height
standoff_height = 60;

board_x0 = (outer_w - board_w) / 2;
board_y0 = (outer_l - board_l) / 2;

// four board corner positions (mounting holes) — measured spacing, not guessed
standoff_pos = [
    [board_x0 + board_hole_inset_x,            board_y0 + board_hole_inset_y],
    [board_x0 + board_w - board_hole_inset_x,  board_y0 + board_hole_inset_y],
    [board_x0 + board_hole_inset_x,            board_y0 + board_l - board_hole_inset_y],
    [board_x0 + board_w - board_hole_inset_x,  board_y0 + board_l - board_hole_inset_y],
];

// ---------- Overall cavity / tray height ----------
cavity_h = standoff_height + board_t + component_clear;
tray_h   = floor_t + cavity_h;

// ---------- Power cord cutout (back wall) ----------
// Sized and positioned relative to the transformer itself, not the case
// floor — center of the hole sits 50mm above the transformer's own base
// (which is raised boss_height above the floor, not sitting on the floor).
cord_w = 35;
cord_h = 15;
cord_center_from_xfmr_bottom = 50;
cord_z = boss_height + cord_center_from_xfmr_bottom - cord_h/2; // bottom of the cutout
cord_x0 = (outer_w - cord_w) / 2;
cord_x1 = (outer_w + cord_w) / 2;

// ---------- USB-A cutout (front wall — board's short/"PY 5*7CM" edge) ----------
// USB Type-A external housing is ~14mm (W) x 7mm (H) — the shielded metal
// shell that has to physically pass through the panel, not just the
// 12x4.5mm contact-opening spec (that's the plug-insertion slot inside it).
// +0.5mm clearance per side per standard panel-mount guidance.
usb_w = 15;
usb_h = 8;
usb_near_edge_offset = 16;                      // hole's near edge starts 16mm from the board's long side
usb_pos_x = board_x0 + board_w - usb_near_edge_offset - usb_w/2;  // center, hole extends inward from there
usb_z0 = floor_t + standoff_height - usb_h;     // top of cutout flush with board's underside

// ---------- Ventilation (over the transformer, not the regulator) ----------
// The LM2596 buck converter runs much cooler than the old 7805 did, so
// venting moved to where the real heat source now is — the transformer's
// core and windings. Positioned using its known height (wedge_height) and
// center (tab_y), on the side wall nearest it.
vent_hole_dia = 1.5;  // small holes — keeps dust/fingers out, still vents
vent_rows     = 5;
vent_cols     = 3;
vent_spacing  = 6;    // center-to-center

// centered over the transformer's footprint (for the lid's top grid, if
// the lid is brought back later)
vent_pos_x = outer_w/2 - (vent_cols - 1) * vent_spacing / 2;
vent_pos_y = tab_y - (vent_rows - 1) * vent_spacing / 2;

side_vent_rows = 3;
side_vent_cols = 3;
// vertically centered on the transformer's body (floor + boss + half its height)
side_vent_z0 = floor_t + boss_height + wedge_height/2 - (side_vent_rows - 1) * vent_spacing / 2;
// horizontally centered on the transformer along the wall (tab_y is its center)
side_vent_y0 = tab_y - (side_vent_cols - 1) * vent_spacing / 2;

// ---------- Sliding lid track ----------
// The lid rides in a channel just above tray_h, captured from lifting out
// by two inward lips running along the left/right walls. The front wall
// is open across its middle at this height so the lid slides in there;
// the back wall stays solid and acts as the closed-end stop.
lid_clear     = 0.4;             // vertical slack so the lid slides freely
lid_channel_h = lid_t + lid_clear;
lip_overhang  = 2.5;             // how far the lip reaches in over the lid edge
lip_thick     = 1.5;             // thickness of the capturing lip material
case_h        = 95;   // fixed per spec

track_h = lid_channel_h + lip_thick;   // total height the slide mechanism needs
interior_top = case_h - track_h;       // where the main cavity stops and the track begins
// sanity check use: interior_top should stay comfortably above floor_t +
// standoff_height + board_t (currently 2+60+1.6=63.6) — if you shrink
// case_h later, verify interior_top hasn't dropped below that.

lid_slide_gap = 0.4;             // horizontal clearance, lid vs. side walls
lid_w = outer_w - 2*wall - lid_slide_gap;      // spans the full inner width
lid_l = outer_l - 2*wall - lid_slide_gap;      // slides flush front to back

// small dome the lid clicks over right before it's fully closed — keeps
// it from creeping open under vibration without needing a screw
detent_r    = 1;
detent_pos_y = outer_l - wall - 6; // just short of the back stop

// ============================================================
// Modules
// ============================================================

module slot_hole(a, b, height) {
    // stadium/slot shape — straight sides, rounded ends — via hull of two
    // circles, not a smooth ellipse. a = width, b = overall length.
    r = a / 2;
    d = b - a; // distance between the two end-circle centers
    hull() {
        translate([0, -d/2, 0]) cylinder(h = height, r = r, $fn = 48);
        translate([0,  d/2, 0]) cylinder(h = height, r = r, $fn = 48);
    }
}

module solid_boss(x, y, dia, height, z0 = floor_t) {
    // plain post, no internal hole — used where the hole needs to cut
    // through the floor too (see base_tray's outer difference)
    translate([x, y, z0])
        cylinder(h = height, d = dia, $fn = 32);
}

module screw_boss(x, y, dia, height, pdia, z0 = floor_t) {
    translate([x, y, z0])
        difference() {
            cylinder(h = height, d = dia, $fn = 32);
            translate([0, 0, -1])
                cylinder(h = height + 2, d = pdia, $fn = 32);
        }
}

module ramp_wedge_front_back(x0, x1, y_contact, direction, run, height) {
    // direction: +1 extends toward +y (back wall side), -1 toward -y (front wall side)
    y_far = y_contact + direction * run;
    hull() {
        translate([x0, y_contact, floor_t])
            cube([x1 - x0, 0.01, height]);          // vertical face — against the transformer
        translate([x0, min(y_contact, y_far), floor_t])
            cube([x1 - x0, abs(run) + 0.01, 0.01]); // flat face — along the floor
    }
}

module transformer_wedges() {
    ramp_wedge_front_back(xfmr_x0, xfmr_x1, xfmr_y0, -1, wedge_run, wedge_height); // front side
    ramp_wedge_front_back(xfmr_x0, xfmr_x1, xfmr_y1,  1, wedge_run, wedge_height); // back side
}

module vent_grid_z(x0, y0, rows, cols, dia, spacing, thickness) {
    // punches a grid of holes straight through a horizontal panel (lid)
    for (i = [0:rows-1])
        for (j = [0:cols-1])
            translate([x0 + i*spacing, y0 + j*spacing, -1])
                cylinder(h = thickness + 2, d = dia, $fn = 16);
}

module vent_grid_x(x_wall, y0, z0, rows, cols, dia, spacing, thickness) {
    // punches a grid of holes straight through a vertical side wall
    for (i = [0:rows-1])
        for (j = [0:cols-1])
            translate([x_wall - 1, y0 + i*spacing, z0 + j*spacing])
                rotate([0, 90, 0])
                    cylinder(h = thickness + 2, d = dia, $fn = 16);
}

module lip(x, w) {
    // inward-facing shelf that overhangs the lid's edge, capturing it
    translate([x, 0, interior_top + lid_channel_h])
        cube([w, outer_l - wall, lip_thick]);
}

module detent_bump() {
    // small dome on the channel floor the lid snaps past when fully closed
    translate([outer_w/2, detent_pos_y, interior_top])
        sphere(r = detent_r, $fn = 24);
}

module base_shell() {
    difference() {
        cube([outer_w, outer_l, case_h]);

        // hollow interior for components — stops at interior_top, NOT
        // case_h, since the slide track occupies the space above that
        translate([wall, wall, floor_t])
            cube([outer_w - 2*wall, outer_l - 2*wall, interior_top - floor_t + 1]);

        // slide channel — open through the front wall, stops at the back wall
        translate([wall, -1, interior_top])
            cube([outer_w - 2*wall, outer_l - wall + 1, lid_channel_h]);

        // space above the channel, open except where the lips sit — also
        // open through the front so the lid can pass under the lips as
        // it's inserted
        translate([wall, -1, interior_top + lid_channel_h])
            cube([outer_w - 2*wall, outer_l - wall + 1, lip_thick]);

        // power cord cutout, back wall (y = outer_l)
        translate([(outer_w - cord_w)/2, outer_l - wall - 1, floor_t + cord_z])
            cube([cord_w, wall + 2, cord_h]);

        // USB-A cutout, front wall (board's short edge)
        translate([usb_pos_x - usb_w/2, -1, usb_z0])
            cube([usb_w, wall + 2, usb_h]);

        // ventilation — right wall, positioned over the transformer's body
        vent_grid_x(outer_w, side_vent_y0, side_vent_z0,
                    side_vent_rows, side_vent_cols, vent_hole_dia, vent_spacing, wall);
    }
}

module base_tray() {
    difference() {
        union() {
            base_shell();

            // transformer mounting bosses — solid for now; the through-hole
            // is cut below, after the union, so it passes through the floor too
            solid_boss(tab_x1, tab_y, boss_dia, boss_height);
            solid_boss(tab_x2, tab_y, boss_dia, boss_height);

            // transformer retaining wedges
            transformer_wedges();

            // perfboard standoffs — screwed from ABOVE, so these keep their
            // own blind pilot hole (drilled only into the post, not the floor)
            for (p = standoff_pos)
                screw_boss(p[0], p[1], standoff_dia, standoff_height, standoff_pilot_dia);

            // sliding lid capture lips, left and right
            lip(wall, lip_overhang);
            lip(outer_w - wall - lip_overhang, lip_overhang);

            // closed-position detent
            detent_bump();
        }

        // transformer screw holes — clear through-holes from the case's
        // bottom exterior, up through the floor and the full boss height,
        // so the screw is driven in from underneath and threads up into
        // the transformer's tab. Stadium/slot shape, long axis pointing
        // toward the other hole (matches the "closer ends" measurement).
        translate([tab_x1, tab_y, -1])
            rotate([0, 0, 90])
                slot_hole(tab_hole_a, tab_hole_b, floor_t + boss_height + 2);
        translate([tab_x2, tab_y, -1])
            rotate([0, 0, 90])
                slot_hole(tab_hole_a, tab_hole_b, floor_t + boss_height + 2);
    }
}

module lid() {
    tab_w = 16;
    tab_r = 3;      // corner rounding on the pull tab
    scoop_r = 7;    // radius of the shallow thumb-scoop sphere
    scoop_depth = 0.9; // how deep it cuts into the top surface — stays shy of lid_t

    difference() {
        union() {
            cube([lid_w, lid_l, lid_t]);
            // rounded pull tab, sticks out a little past the front edge
            translate([lid_w/2, -3 + tab_r, 0])
                hull() {
                    translate([-tab_w/2 + tab_r, 0, 0]) cylinder(r = tab_r, h = lid_t, $fn = 32);
                    translate([ tab_w/2 - tab_r, 0, 0]) cylinder(r = tab_r, h = lid_t, $fn = 32);
                    translate([-tab_w/2 + tab_r, 3, 0]) cylinder(r = tab_r, h = lid_t, $fn = 32);
                    translate([ tab_w/2 - tab_r, 3, 0]) cylinder(r = tab_r, h = lid_t, $fn = 32);
                }
        }

        // ventilation grid — directly over the transformer
        vent_grid_z(vent_pos_x, vent_pos_y, vent_rows, vent_cols, vent_hole_dia, vent_spacing, lid_t);

        // shallow thumb-scoop — a smooth dimple, not a hole through the material
        translate([lid_w/2, 0, lid_t - scoop_depth + scoop_r])
            sphere(r = scoop_r, $fn = 48);
    }
}

// ============================================================
// Render — export TWO separate STLs, one at a time
// ============================================================
// This file makes ONE part per render, controlled by the flag below.
// To get both STLs:
//   1. Set export_part = "base", press F6, File > Export > Export as STL
//      (save as e.g. base_tray.stl)
//   2. Change export_part = "lid", press F6 again, export again
//      (save as e.g. lid.stl)
// Each is a separate, independent solid — they are never unioned
// together, so there's no need to separate them afterward.
export_part = "base";   // "base" or "lid"

if (export_part == "base")
    base_tray();

if (export_part == "lid")
    lid();
