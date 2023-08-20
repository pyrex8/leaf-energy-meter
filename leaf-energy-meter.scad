include <boxes.scad>
// Dimensions in mm

difference(){  
      
    
translate([0,0,2.5])
    cube([30,30,5],true);

// pcb
translate([0,0,3.5])
    cube([28,28,5],true);

// screen
translate([0,2,2])
    cube([24,13,5],true);
    
// cable
translate([0,-14,5])
    cube([10,10,5],true);
}
