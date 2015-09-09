-- DESTECS (Design Support and Tooling for Embedded Control Software)
-- Grant Agreement Number: INFSO-ICT-248134
-- Seventh Framework Programme
-- WaterTank project: introductory project to the DESTECS tool.
-- More details can be found at DESTECS User Manual, chapter 6.

-- File watertank.csc: configuration file that defines the interfaces between
-- the logical model and the simulation engine.


-- Shared design parameters
sdp real maxlevel;
sdp real minlevel; 

-- Monitored variables (seen from the DE controller)
monitored real level;

-- Controlled variables (seen from the DE controller)
controlled bool valve; 
