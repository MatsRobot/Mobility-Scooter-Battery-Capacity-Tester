# Mobility-Scooter-Battery-Capacity-Tester
This device was used to test the remaining capacity of Lead Acid Battery used by Mobility Scooters

The device uses two 12V-55W light bulbs as the load, using 5A current to take the fully charged battery to 50% capacity which is the safe discharge limit at 12.23V. Arduino is used to log the voltages of batteries at every measurement to an SD card and also switches off the load automatically during the measurements as the quoted voltages relates to the battery voltage without load and then switch them off once 50% is reached.


Safety considerations MUST be taken to avoid battery shortage, light bulb shortage or exposure to heat and light from the light bulbs.
![Screenshot 2024-10-22 075307](https://github.com/user-attachments/assets/4ad89288-b86b-472c-92be-2ce9b4178050)

The following safety steps were carried taken during the test:

1: Three 10A inline Fuses were used directly after the terminals to remove the possibility of accidentally shorting the batteries. 

2: The light bulbs were placed in separate glass housings to avoid shortage, exposure to heat and direct light. External fan was used to cool the housing.

3: The light bulbs do not have common earth, so they were kept sepatate at all times to ensure they didn't touch at any point during the test.


The Tested lead-acid batteries were AGM MU-1 SLD M FT 12v-35AH/20HR from MK battery and the new battery typically reaches the 50% capacity using a 5A discharge load in about 5 hours and gets to 0% capacity just over 6 hours.

![Screenshot 2024-10-16 065214](https://github.com/user-attachments/assets/0141f0c6-091d-4db5-bc24-7959f566fc13)

There are two 12v batteries in Mobility scooters that are connected in series to create the required 24v operating voltage. This provides three accessible  points, 0V, 12V and 24V. To use a common earth circuit the second battery is not independent of the first so to get the voltage of the second battery we need to measure the total voltage of the first+second batteries and subtract it from the voltage of the first battery.

The Lead-acid battery will start getting damaged if discharged over 30%, so there was a significant focus to ensure the voltage doesn't get below that threshold. 
When a lead-battery sits below 50% state of charge, the rate of growth & accumulation of lead sulfate crystals increases substantially and blocks access to the plates for the electrolyte which diminishes battery capacity. 

The battery gets damaged and cannot be reversed fully. This means that the only way to measure the capacity of the battery and the distance that the mobility scooter could travel on a full charge is by discharging it to 50% in a control environment and measure the voltages vs time taken for the discharge.

The circuit was designed to be used with either one (12v) or two lead-acid batteries in series (24v).

We refer to typical Lead-acid batteries as 12v, however the actual voltage values vs capacity are:

  12.89v = 100% , battery full and standing for a while

  12.65 = 80% , safe discharge level

  12.23 = 50% , below this level battery start to get damaged

  11.96 = 30% , below this value the battery will not recover fully

  11.63 = 0% , Totally Dead from capacity point of view

This project used module 'RFID_Clocking_Machine_V4.4' for the data logging and formed the base of the unit. 

The additional following pins were assigned to the Battery Tester and a small circuit was designed to take the voltage from the batteries to be measured.

![Screenshot 2024-10-15 174659](https://github.com/user-attachments/assets/4e2f7d95-faed-4111-bf52-be06c072d546)

There is no value for the Cold Cranking Amp (CCA) for these batteries quoted in the datasheets, but the measurements of the new purchased NP50-12 (50Ah) showed the CCA as 378 and the CA to be 478. I based the initial rating as what I found as the maximum amps for the 23(AH) battery which was CA 330, however for the initial measurements of a new battery this could be any value as it only uses it for comparison to what it reads. I used 'TOPDON - ArtiBattery101' for these measurements.

![Screenshot 2024-10-19 071327](https://github.com/user-attachments/assets/6e741641-0d63-4834-a514-c55d0aab8508)
![Screenshot 2024-10-20 065821](https://github.com/user-attachments/assets/f86364e3-aeb3-43ca-9f73-6bb18e48adf9)



I couldn't find the typical current used by the scooter motors on any of their specs, so I've measured the current taken through the batteries and I found that it varies a lot depending on the path, type of surface, weight etc, but what I wanted was an idea of the current and these are the results:

Maximum measured current was 21A, this was riding on the grass.

The typical ride takes 5A to 10A.

The average current on a normal ride was 5A.


![Screenshot 2024-10-20 070919](https://github.com/user-attachments/assets/08e3c8ad-65a4-4ee9-978d-c02e7e053a9a)
![Screenshot 2024-10-20 071200](https://github.com/user-attachments/assets/725ae643-56e3-4f92-81d4-a6793f0a1885)


Using 2 12V-55W light bulbs adequately simulate the average load, so the measured capacity in hours using the test module was very close to what the scooter was doing.


