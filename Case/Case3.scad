
$fn=50;


//
//
//
innerRadius     = 40;
weightRadius    = 29;


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
                rotate_extrude(convexity = 5) translate([innerRadius, 0, 0]) circle(r = 2);
                cylinder(h=4, r1=innerRadius, r2=innerRadius, center=true);
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
    //color([0,1,0])
    union()
    {
        difference()
        {
            difference()
            {
                //
                // External body
                //
                translate([0,0,-3.3])
                  cylinder(h=20.5, r1=innerRadius+2, r2=innerRadius+2, center=true);
                
                //
                // Internal cutout.
                //
                translate([0,0,-3.3])
                  cylinder(h=21, r1=innerRadius, r2=innerRadius, center=true);
            };
            
            //
            // Rear port cutout.
            //
            //translate([0,40,0]) rotate([90,0,90]) cylinder(h=55, r1=5, r2=5, center=true);
            color([1,0,0])
            translate([0,40,-7]) rotate([90,0,90]) cube(size=[20,15,38], center=true);

            //
            // Fornt port cutout.
            //
            //translate([0,-40,0]) rotate([90,0,90]) cylinder(h=55, r1=5, r2=5, center=true);
            color([1,0,0])
            translate([0,-40,-5]) rotate([90,0,90]) cube(size=[20,10,28], center=true);
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
    union()
    {
        rotate([0,0,90]) color([.8,0,0.8]) translate([-106,-159,-5]) 
            import("../Board/ReactorBoardEdgeCutsWithMountingHoles2.dxf");

        color([0,0.5,0.7]) translate([0,10,-11]) cube(size=[46,48,1], center=true);
    }
}

//
//
//
module Posts()
{
}

//
//
//
module Bottom()
{
    //color([1,0,0]) 
    translate([0,0,-10])
    difference()
    {
        union()
        {
            rotate_extrude(convexity = 5) translate([innerRadius, 0, 0]) circle(r = 2);
            cylinder(h=4, r1=innerRadius, r2=innerRadius, center=true);

            //color([1,0,0]) 
            {
            translate([-27.5,-19.8,5]) cylinder(h=13, r1=3, r2=3, center=true);
            translate([ 27.8,-19.8,5]) cylinder(h=13, r1=3, r2=3, center=true);
            translate([-27.5, 19.8,5]) cylinder(h=13, r1=3, r2=3, center=true);
            translate([ 27.8, 19.8,5]) cylinder(h=13, r1=3, r2=3, center=true);
            }

            translate([0,0,1]) cylinder(h=2, r1=innerRadius+2, r2=innerRadius+2, center=true);

            difference()
            {
                translate([0,0,2]) cylinder(h=2, r1=innerRadius, r2=innerRadius, center=true);
                translate([0,0,2]) cylinder(h=3, r1=innerRadius-5, r2=innerRadius-5, center=true);
            }
            
        }
        
        //
        // Internal lid cutout for circular diffusion sheet.
        //
        translate([0,0,1]) cylinder(h=3, r1=weightRadius, r2=weightRadius, center=true);
        
        //
        // Bottom logo
        //
        translate([0,0,-2.5])
        {
            rotate([0,180,0])
            {
                linear_extrude(height = 2, center = true, convexity = 10, twist = 0)
                scale([0.8,0.8,1]) translate([-113,-157,0]) import("../../Media/BlockWorksLogo.dxf");
            }
        }
    }
}


module ClosedCase()
{
    Body();
    translate([0,0,7]) CaseTopWithLogo();
    translate([0,0,3]) Board();
    translate([0,0,-5]) Bottom();
}

module OpenCase()
{
    Body();
    translate([0,0,7]) CaseTopWithLogo();
    translate([0,0,-13]) Board();
    translate([0,0,-20]) Bottom();
}


ClosedCase();
//OpenCase();


