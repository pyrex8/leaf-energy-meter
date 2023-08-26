// Dimensions in mm

difference(){  
      
// main block    
translate([0,0,3])
    cube([30,48,5.5],true);

// pcb
translate([0,0,3.5])
    cube([28,28,5],true);

// screen
translate([0,1.25,2])
    cube([24,13,5],true);
    
// cable
translate([0,-14,6])
    cube([10,15,5],true);
    
// support bottom
translate([0,-24.5,3])
rotate([0,0,-7])
    cube([36,10,6],true);

// support top
translate([0,23.6,3])
rotate([0,0,-13])
    cube([36,10,6],true);    
}