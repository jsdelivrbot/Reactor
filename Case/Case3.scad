
$fn=200;

include <ReactorLogo.scad>



//
//
//
module CaseTopWithLogo()
{
	difference()
	{
		cylinder(h=2, r1=40, r2=40, center=true);
		translate([0,0,-10]) scale([0.3, 0.3, 4.0]) ReactorLogo(10.0);
	}
}


//
//
//
module Body()
{
    difference()
    {
        cylinder(h=15, r1=40, r2=40, center=true);
        cylinder(h=16, r1=38, r2=38, center=true);
    };
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
translate([0,0,-10]) Board();



