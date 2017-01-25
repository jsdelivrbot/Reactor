
$fn=100;


//
// pstoedit -dt -f dxf:-polyaslines -mm ../../Media/Reactor.eps ../../Media/Reactor.dxf
//
module Logo()
{
    linear_extrude(height = 10, center = true, convexity = 10, twist = 0)
    scale([8,8,0]) translate([-4.27,-6.55,0]) import("../../Media/Reactor.dxf");
}


//
//
//
module CaseTopWithLogo()
{
    difference()
    {
        difference()
        {
            union()
            {
                rotate_extrude(convexity = 5) translate([38, 0, 0]) circle(r = 2);
                cylinder(h=4, r1=38, r2=38, center=true);
            }
            
            translate([0,0,0]) Logo();
        }
        
        //
        // Internal lid cutout for circular diffusion sheet.
        //
        translate([0,0,-1]) cylinder(h=3, r1=20, r2=20, center=true);
    }
}


//
//
//
module Body()
{
    union()
    {
        difference()
        {
            difference()
            {
                //
                // External body
                //
                cylinder(h=15, r1=40, r2=40, center=true);
                
                //
                // Internal cutout.
                //
                cylinder(h=16, r1=38, r2=38, center=true);
            };
            
            //
            // Rear port cutout.
            //
            translate([0,40,0]) rotate([90,0,90]) cylinder(h=55, r1=5, r2=5, center=true);

            //
            // Fornt port cutout.
            //
            translate([0,-40,0]) rotate([90,0,90]) cylinder(h=55, r1=5, r2=5, center=true);
        }

        //
        // light separation bars.
        //
        translate([0,-0.2,6]) cube(size=[50,1,3], center=true);
        rotate([0,0,90]) translate([0,0.3,6]) cube(size=[50,1,3], center=true);
    }

}

//
//
//
module Board()
{
    rotate([0,0,90]) color([.8,0,0.8]) translate([-40,-40,-5]) import("../Board/ReactorBoardEdgeCuts.dxf");
}



Body();
translate([0,0,7]) CaseTopWithLogo();
translate([0,0,8]) Board();

//translate([0,0,10]) Logo();



//translate([0,37,0]) rotate([90,0,90]) cylinder(h=55, r1=5, r2=5, center=true);


/*
translate([0,0,-7]) 
union()
{
    rotate_extrude(convexity = 10) translate([38, 0, 0]) circle(r = 2);
    cylinder(h=4, r1=38, r2=35, center=true);
}
*/

