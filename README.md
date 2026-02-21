# 🔋 Mobility Scooter Battery Capacity Tester

This device was engineered to scientifically test the remaining capacity of Lead-Acid batteries used in Mobility Scooters. It utilizes a simulated real-world load to determine if a battery can maintain the voltage required for reliable operation without crossing the 50% damage threshold.

---

<table>
  <tr>
    <td width="60%">
      <h2>🚀 System Logic & Automation</h2>
      <p>The device uses two 12V-55W light bulbs as a load, drawing a constant 5A current. An Arduino logs battery voltages to an SD card at set intervals.</p>
      <p><b>Smart Voltage Checking:</b> To get a true reading of how much "fuel" is left in the battery, the machine has to pause. It briefly turns off the power-draw every minute to take a "Resting Voltage" measurement. This is much more accurate than measuring while the motor is running, which can make a battery look emptier than it actually is.</p>
    </td>
    <td width="40%">
      <img src="https://github.com/user-attachments/assets/4ad89288-b86b-472c-92be-2ce9b4178050" alt="Main Device Interface" />
    </td>
  </tr>
</table>

---

## ⚠️ Safety Protocols
To manage the high current (5A+) and significant heat from the halogen bulbs, the following steps were implemented:
* **Fusing:** Three 10A inline fuses are placed directly after the terminals to prevent accidental battery shorts.
* **Thermal Control:** Light bulbs are housed in separate glass enclosures with an external fan to manage heat and light exposure.
* **Electrical Isolation:** The bulbs lack a common earth and are kept physically separate to ensure they never touch during the test.

---

## 📈 Capacity Analysis
The baseline testing was performed on **AGM MU-1 SLD M FT 12V-35AH/20HR** batteries (MK Battery). A new battery typically reaches 50% capacity in 5 hours under a 5A load and hits 0% (total depletion) in just over 6 hours.



### 🔄 24V Series Configuration logic
Mobility scooters operate at 24V using two 12V batteries in series. To monitor these with a common-earth Arduino circuit:
* **Battery 1 (B1):** Measured directly from 0V to the 12V center tap.
* **Battery 2 (B2):** Measured by taking the total series voltage (0V to 24V) and subtracting the B1 value.
* **Formula:** $V_{B2} = V_{total} - V_{B1}$



---

## 🧪 Battery Chemistry & Maintenance
Sulfation is the primary enemy of Lead-Acid longevity. When a battery sits below 50% State of Charge (SoC), **lead sulfate crystals** accumulate on the plates. This blocks the electrolyte and permanently diminishes capacity. To maintain your batteries, use the following resting voltage reference:

| Resting Voltage | Capacity % | Status |
| :--- | :--- | :--- |
| **12.89V** | 100% | Full Charge |
| **12.65V** | 80% | Safe Operating Range |
| **12.23V** | 50% | **Critical: Sulfation Damage Begins** |
| **11.96V** | 30% | Severe Depletion (Damage Unlikely to Reverse) |
| **11.63V** | 0% | Totally Dead |

---

## 🔍 Real-World Diagnostic Data

<table>
  <tr>
    <td width="55%">
      <h3>CCA & Health Baselines</h3>
      <p>Using a <b>TOPDON ArtiBattery101</b>, new <b>NP50-12 (50Ah)</b> batteries showed a CCA of 378 and a CA of 478. Initial comparison ratings were based on 23Ah batteries (CA 330) to establish a baseline for resistance growth.</p>
    </td>
    <td width="45%">
      <img src="https://github.com/user-attachments/assets/6e741641-0d63-4834-a514-c55d0aab8508" width="180"/>
      <img src="https://github.com/user-attachments/assets/f86364e3-aeb3-43ca-9f73-6bb18e48adf9" width="140"/>
    </td>
  </tr>
  <tr>
    <td width="55%">
      <h3>Motor Load Measurements</h3>
      <p>Scooter current draw varies significantly by terrain. Understanding this helped calibrate the 5A tester load:</p>
      <ul>
        <li><b>Average/Typical Ride:</b> 5A</li>
        <li><b>Variable Range:</b> 5A - 10A</li>
        <li><b>Peak Load (Grass):</b> 21A</li>
      </ul>
    </td>
    <td width="45%">
      <img src="https://github.com/user-attachments/assets/08e3c8ad-65a4-4ee9-978d-c02e7e053a9a" width="180"/><br>
      <img src="https://github.com/user-attachments/assets/725ae643-56e3-4f92-81d4-a6793f0a1885" width="180"/>
    </td>
  </tr>
</table>

---

## 🛠️ Implementation Details
This project utilizes the **RFID_Clocking_Machine_V4.4** module as a data-logging backbone. A custom voltage-divider circuit was designed to safely interface the high battery voltages with the Arduino analog pins.



<p align="center">
  <img src="https://github.com/user-attachments/assets/4e2f7d95-faed-4111-bf52-be06c072d546" alt="Wiring and Pin Assignments" />
</p>

---

<small>© 2026 MatsRobot | Licensed under the [MIT License](https://github.com/MatsRobot/matsrobot.github.io/blob/main/LICENSE)</small>
