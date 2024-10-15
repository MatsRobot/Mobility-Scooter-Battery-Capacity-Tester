# Mobility-Scooter-Battery-Capacity-Tester
Device to test the remaining capacity of Lead Acid Battery used by Mobility Scooters

In Mobility scooters there are two 12v batteries connected in series to create the required 24v operating voltage. hence there are three accessible points, 0V, 12V and 24V. To get the voltage of the second battery we need to measure the total voltage of the first+second battery and subtract it from the voltage of the first battery.

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

![Screenshot 2024-10-15 174659](https://github.com/user-attachments/assets/feb719b0-c4c7-44c2-a61d-dc4eb0f09a49)
