include <boxes.scad>
// Dimensions in mm

difference(){  
      
    
translate([0,0,3])
    cube([30,32,5.5],true);

// pcb
translate([0,0,3.5])
    cube([28,28,5],true);

// screen
translate([0,1.25,2])
    cube([24,13,5],true);
    
// cable
translate([0,-14,6])
    cube([10,10,5],true);
}

// support bottom
translate([14,-17,3])
    cube([2,6,5.5],true);

// support top
translate([-14,19,3])
    cube([2,9,5.5],true);