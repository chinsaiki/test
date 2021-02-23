# dmidecode 指令

## `dmidecode -t`

```bash
dmidecode
dmidecode -t 1
dmidecode -t system
```
其中，`1`和`system`可以替换为下列一些：

```
	Type	Description
	0	BIOS Information
	1	System Information
	2	Baseboard (or Module) Information
	3	System Enclosure or Chassis
	4	Processor Information
	5	Memory Controller Information (Obsolete)
	6	Memory Module Information (Obsolete)
	7	Cache Information
	8	Port Connector Information
	9	System Slots
	10	On Board Devices Information
	11	OEM Strings
	12	System Configuration Options
	13	BIOS Language Information
	14	Group Associations
	15	System Event Log
	16	Physical Memory Array
	17	Memory Device
	18	32-Bit Memory Error Information
	19	Memory Array Mapped Address
	20	Memory Device Mapped Address
	21	Built-in Pointing Device
	22	Portable Battery
	23	System Reset
	24	Hardware Security
	25	System Power Controls
	26	Voltage Probe
	27	Cooling Device
	28	Temperature Probe
	29	Electrical Current Probe
	30	Out-of-Band Remote Access
	31	Boot Integrity Services (BIS) Entry Point
	32	System Boot Information
	33	64-Bit Memory Error Information
	34	Management Device
	35	Management Device Component
	36	Management Device Threshold Data
	37	Memory Channel
	38	IPMI Device Information
	39	System Power Supply
	40	Additional Information
	41	Onboard Devices Extended Information
	42	Management Controller Host Interface
	126	Inactive
	127	End-of-Table
	128-255	Available for system- and OEM- specific information
```
