# Mobility-Scooter-Battery-Capacity-Tester
This device tests the remaining capacity of Lead Acid Battery used by Mobility Scooters

It uses two 12V-55W light bulbs as the load, using 5A current to take the fully charged battery to 50% capacity which is the safe discharge limit at 12.23V. Arduino is used to log the voltages of battries to an SD card and also switch off the load automatically during the measurements as the voltages relates to the battery voltage without and load and then switch them off once 50% is reached.

The Tested lead acid batteries were Lucas LSLC22-12 and the new battery typically reaches the 50% capacity using a 5A discharge load in about 5 hours and gets to 0% capacity just over 6 hours.

![Screenshot 2024-10-16 065214](https://github.com/user-attachments/assets/0141f0c6-091d-4db5-bc24-7959f566fc13)

There are two 12v batteries in Mobility scooters that are connected in series to create the required 24v operating voltage. This provides three accessible  points, 0V, 12V and 24V. To use a common earth circuit the second battery is not independant of the first so to get the voltage of the second battery we need to measure the total voltage of the first+second batteries and subtract it from the voltage of the first battery.

The Lead acid battery will get damages if discharged over 30%, so the there is a significant focus of ensuring that the voltage . 
When a lead battery sits below 50% state of charge, the rate of growth & accumulation of lead sulfate crystals increases substantially and blocks access to the plates for the electrolyte which diminishes battery capacity. 

The battery gets damaged that cannot be reversed fully. This means that the only way to measure the capacity of the battery and the distance that the mobility scooter could travel on a full charge is by discharging it to 50% in a control environment and measure the voltages vs time taken for the discharge.

The circuit was designed to be used with either one (12v) or two lead acid batteries in series (24v).

With Lead acid batteries we talk 12v batteries but the actual voltage values vs capacity are:

  12.89v = 100% // Battery full and standing for a while

  12.65 = 80% // Safe discharge level

  12.23 = 50% // below this level battery start to get damaged

  11.96 = 30% // below this value the battery will not recover fully

  11.63 = 0% // Totally Dead from capacity point of view

This project uses module 'RFID_Clocking_Machine_V4.4' for the data logging and forms the base of the unit. 

The additional following pins are assigned to the Battery Tester and a small circuit was designed to take the voltage from the batteries to be measured.

![Screenshot 2024-10-15 174659](https://github.com/user-attachments/assets/4e2f7d95-faed-4111-bf52-be06c072d546)

There is no value for the Cold Cranking Amp (CCA) for these batteries quoted in the datasheet, but the measurements of the new NP50-12 (50Ah) shows that the CCA for a new battery is 378 and the CA is 473, and estimated value for the 23(AH) battery is CCA 330.


