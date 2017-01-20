
$fa = 0.5;
$fs = 0.5;

//include <BlockWorks.scad>
include <ReactorLogo.scad>


//
//
//
module FrontCaseHoles(size)
{
   union()
   {
	
   		translate([5,(-size)-10.0, pcbHeight])
		{
				linear_extrude(height=10.0)
   	   			{
						square([20.0,10.0]);
				}
		}

   		translate([-25,(-size)-10.0, pcbHeight])
		{
				linear_extrude(height=10.0)
   	   			{
						square([20.0,10.0]);
				}
		}


   }
}


//
//
//
module RearCaseHoles(size)
{
   union()
   {

   		translate([-10,(size)-0.0, pcbHeight])
		{
				linear_extrude(height=5.0)
   	   			{
						square([5.0,10.0]);
				}
		}


   		translate([10,(size)-0.0, pcbHeight])
		{
				linear_extrude(height=5.0)
   	   			{
						square([5.0,10.0]);
				}
		}


   }
}


//
//
//
module CaseTop(size,height,radius, wallWidth)
{
	difference()
	{
		translate([-size/2,-size/2, 0])
		{
			difference() 
			{
				linear_extrude(height=height)
   	   			{
					minkowski() 
   					{	
						circle(radius);
						square([size,size]);
					}
				}
	

				linear_extrude(height=height-wallWidth)
   	   			{
					minkowski() 
   					{
						circle(radius-wallWidth);
						square([size,size]);
					}
				}
	
			}	
 		}


		FrontCaseHoles(size);


		RearCaseHoles(size);
	}


}


//
//
//
module CaseTopWithLogo(size,height,radius, wallWidth)
{
	difference()
	{
		CaseTop(size, height, radius, wallWidth);
		translate([0,0,height-0.5]) scale([0.3, 0.3, 1.0]) ReactorLogo(2.0);
	}
}



//
//
//
module PCBStandOff(size,height,radius, wallWidth, offsetX, offsetY)
{
	translate([offsetX, offsetY, pcbHeight])
	{
		linear_extrude(height=height-wallWidth-pcbHeight)
		{
			circle(4);
		}
	}
}


//
//
//
module PCBStandOffHole(size,height,radius, wallWidth, offsetX, offsetY)
{
	pcbHeight		= 2;

	translate([offsetX, offsetY, pcbHeight])
	{
		linear_extrude(height=height)
		{
			circle(2);
		}
	}
}







//
//
//
module PCBStandOffHoles(size,height,radius, wallWidth, standoffOffset)
{
	offset 		= standoffOffset;

	union()
	{
		PCBStandOffHole(size,height,radius, wallWidth, -offset,-offset);
		PCBStandOffHole(size,height,radius, wallWidth, offset,-offset);
		PCBStandOffHole(size,height,radius, wallWidth, offset,offset);
		PCBStandOffHole(size,height,radius, wallWidth, -offset,offset);
	}
}

//
//
//
module PCBStandOffs(size,height,radius, wallWidth, standoffOffset)
{
	offset 		= standoffOffset;

	difference()
	{
		union()
		{
			PCBStandOff(size,height,radius, wallWidth, -offset,-offset);
			PCBStandOff(size,height,radius, wallWidth, offset,-offset);
			PCBStandOff(size,height,radius, wallWidth, offset,offset);
			PCBStandOff(size,height,radius, wallWidth, -offset,offset);
		}

		PCBStandOffHoles(size,height,radius, wallWidth, standoffOffset);
	}
}



//
//
//
module CaseBottom(size,height,radius, wallWidth, standoffOffset, bottomThickness)
{
	difference()
	{
		translate([-size/2,-size/2, -(bottomThickness-wallWidth)])
		{
			linear_extrude(height=bottomThickness)
	   	   	{
				minkowski() 
	   			{
					circle(radius-wallWidth);
					square([size,size]);
				}
			}
		}


		translate([0,0, -(height-5)])
		{
			PCBStandOffHoles(size,height,radius, wallWidth, standoffOffset);
		}

	}
}


//
//
//
module Core407PCB(size,height,radius, wallWidth)
{
	b1 	= 79;
	w1 	= 56;
	translate([-w1/2, -b1/2, 0])
	{
		linear_extrude(height=15)
   		{
			square([w1,b1]);
		}
	}
}



//
//
//
module MotherboardPCB(size,height,radius, wallWidth, standoffOffset)
{
	w1 	= (standoffOffset*2)+8;
	b1 	= (standoffOffset*2)+8;
	w2 	= size;
	b2 	= size+((radius-wallWidth)*2);

	echo("Motherboard size = ",w1,"x",b1, " and ",w2,"x",b2);

	difference()
	{
		union()
		{
			translate([-w1/2, -b1/2, 0])
			{
				linear_extrude(height=pcbHeight)
   				{
					square([w1,b1]);
				}
			}

			translate([-w2/2, -b2/2, 0])
			{
				linear_extrude(height=pcbHeight)
   				{
					square([w2,b2]);
				}
			}
		}

		translate([0,0, -(height-5)])
		{
			PCBStandOffHoles(size,height,radius, wallWidth, standoffOffset);
		}
	}
}


pcbHeight		= 2;
wallWidth = 3;
internalHeight = 25;
size = 50;
height = (wallWidth*2)+internalHeight;
radius = 30;
standoffOffset = (size/2) + 15;
union()
{
	CaseTopWithLogo(size, height, radius, wallWidth);
	PCBStandOffs(size, height, radius, wallWidth, standoffOffset);
/*
	translate([0, 0, pcbHeight-20])
	{
		Core407PCB(size, height, radius, wallWidth);
	}

	translate([0, 0, 0-20])
	{
		MotherboardPCB(size, height, radius, wallWidth, standoffOffset);
	}

	translate([0, 0, -40])
	{
		CaseBottom(size, height, radius, wallWidth, standoffOffset, wallWidth+2);
	}
*/
}




