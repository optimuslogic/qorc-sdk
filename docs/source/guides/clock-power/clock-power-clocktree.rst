
QORC SDK CLOCK/POWER INFRASTRUCTURE : CLOCK TREE
================================================

|PRE RELEASE|


.. contents::

Introduction
------------

A few key points to remember during any discussion about the clock infrastructure in the EOSS3:

- All of the clocks in the EOSS3 are (practically) always derived from the HSOSC
  
  The clock source can be switched to the REFCLK (32768 Hz) but practically, it is not used.

- The HSOSC frequency is derived from the REFCLK (32768 Hz) with a multiplier, with the formula below:

  :code:`HSOSC = (3 + OSC_CTRL_1__prog) * 32768`

  where, OSC_CTRL_1__prog is the value of the :code:`prog` bitfield [12:0] of 
  the :code:`OSC_CTRL_1` register (0x40005484)

  OSC_CTRL_1__prog can have a max value of 8191, having 13 bits.

- Due to the fact that HSOSC is always derived from 32768 Hz multiplied by an integer, we will always have
  clocks which relate to base :code:`1024` rather than base :code:`1000`

  This means that, in the SW, when values like :code:`HSOSC_72MHz` are seen, then it is not the conventional
  72000000 Hz but instead, is 72000 * 1024 = 73728000 Hz (73.728 MHz) in practice.

  To avoid confusion, in this and related clock docs, we will always explicitly indicate frequencies.

  In the above case, we would **not** refer to the frequency as 72 MHz, but as 73728000 Hz, or 73.728 MHz,
  or 72000 KiHz (using the binary prefix Ki)
  
  :code:`Ki` refers to kibi - meaning 1024 as per the IEC 80000 convention for binary base numbers.

- On a related note, it can also be derived that we would never be able to generate certain
  frequencies exactly. For example, an exact 72 MHz would never be possible to generate in the EOSS3 system.

  Correspondingly, we *may* not always be able to generate other base :code:`1000` frequencies such as 5 MHz.

  This applies to all the domain clocks in the EOSS3, because all the domain clocks are generated by dividing
  down the HSOSC with *integer* dividers.

- Another consequence of all the domain clocks being generated from HSOSC is that any time one of the domain
  clock frequency needs to be changed, and hence HSOSC needs to be changed, it will affect the *other* domain
  clocks as well, leading to more constraints on the HSOSC according to all clock requirements in the system.


For any target clock frequency, we would need to consider 2 aspects:

1. Exact Requirement Calculation: 

   If the target frequency is an exact requirement (for example, PDM) then we need to check that it can
   be produced in the system.

   This can be calculated using the LCM of the REFCLK (32768 Hz) and the target frequency.

   | Example: Obtain 3072000 Hz (3.072 MHz) at, say C10.
   | LCM (32768, 3072000) = 12288000 Hz (12.288 MHz) which is in range for HSOSC (2.5 - 80 MHz)
   | So, this target frequency is possible to be exactly generated
   |

   | Example: Obtain 4800000 Hz (4.8 MHz) at, say C10.
   | LCM (32768, 4800000) = 307200000 Hz (307.2 MHz) which is **NOT** in range for HSOSC (2.5 - 80 MHz)
   | So, this target frequency is **NOT** possible to be exactly generated

   If the target frequency is not an exact requirement(for example SPI, I2C), we should be able to achieve 
   close to the target frequency and this should be ok in most cases.

2. HSOSC Constraints With Other Clocks:

   If we have more than 1 clock, which should be exactly obtained after being divided down from HSOSC, 
   then we can calculate the constraint on HSOSC using the LCM of the clocks and the REFCLK (32768 Hz).

   NOTE: we assume that we have checked all the clocks are possible to achieve using the above method.

   Example: We would like C10 to be a multiple of (or equal to) 3072000 Hz, 
   and C30 to be a multiple of (or equal to) 1024000 Hz.

   LCM (32768,3072000,1024000) = 12288000 (12.288 MHz, 12000 KiHz)

   HSOSC then needs to be a multiple of (or equal to) 12288000 Hz to achieve this combination.


We can use an online tool, such as: https://www.calculator.net/lcm-calculator.html


There are many clocks which we do not (normally) need to check while trying to set the clock frequencies
for any particular application (for example AHB clocks, APB clocks) and we would not look at these in depth.

To understand the clocks which we generally consider, we will take an "example design" approach
which may be easier to digest in a step-by-step manner, rather than taking the system as a whole.

This would be a contrived example, but should be applicable for most practical applications.


Major Blocks in the EOSS3
--------------------------

The major logic blocks/peripherals (depending on the application) that we would usually look at are :

- UART
- AUDIO: PDM, LPSD, I2S
- SPI: SPI1 Master
- SPI: SPI0 Master
- I2C: I2C0 Master, I2C1 Master
- FPGA
- SPI Slave (with a HOST)

We will look at each block in brief, and the domain clocks which provide the clocking to them.

UART
~~~~

The UART clock is sourced from C11 which has a max spec of 10 MHz.

Note that other peripherals sourced from C11 are WDT and TIMER1, and in general there is no need to
think about them specifically w.r.t clocks.

The UART peripheral has a fractional divider, which can divide down 
to produce standard baud rates (115200, 9600 etc.)

There is not much to worry about the baudrate matching, due to the fractional divider 
and (almost) any C11 frequency would do.

:code:`TODO add calculations/preferred frequencies for least error in baud, if any`

Whenever HSOSC changes, C11 will change, and we need to ensure that the UART 
fractional divider is set to output the required baudrate.


AUDIO: PDM, LPSD, I2S
~~~~~~~~~~~~~~~~~~~~~

PDM LEFT, PDM RIGHT, and I2S clocks are derived from C30 with a max spec of 5 MHz.

LPSD clock is derived from C31 (which is derived from C30) with a max spec of 1 MHz, 
typically used at 512 kHz.

| Usually, PDM Clocks have very specific frequency requirements, 
  depending on the microphone spec, and need to be met exactly.
| Same goes for the LPSD clock, and the I2S clock.


SPI: SPI1 Master
~~~~~~~~~~~~~~~~

The SPI1M Clock is derived from C02 clock which has a max spec of 40 MHz.

The C02 goes through a SPI1M BAUDR divider which can have any **even** divider 
value (range: 2 - 16384) to produce the SPI1M clock out.

So, the max SPI1M frequency is 20 MHz (when C02 is at 40 MHz, and BAUDR divider is 2).

Whenever the HSOSC changes, the C02 will change, and correspondingly 
the SPI1M BAUDR divider needs to be set to ensure a specific frequency output.


SPI: SPI0 Master
~~~~~~~~~~~~~~~~

SPI0M is part of the FFE domain, and its clock is derived from the 
C08_X1 clock, which has a max spec of 10 MHz.

This is meant to be interfaced with SPI Sensors (hence the lower max speed) but can also be used for any
SPI peripheral.

SPI0M is interfaced as a Wishbone Slave, and accessed from the M4 via a Wishbone Master.

It has 2 8-bit BAUDR registers to derive the SPI clock from the C08_X1 clock.


I2C: I2C0 Master, I2C1 Master
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

I2C0 and I2C1 are part of the FFE domain, and their clocks are derived from the 
C08_X1 clock, which has a max spec of 10 MHz.

Both of these are interfaced as a Wishbone Slave, and accessed from the M4 via a Wishbone Master.

Each of these has 2 8-bit PRESCALE registers to derive the I2C clock from the C08_X1 clock.

:code:`TODO add calculation details`


FPGA
~~~~

The FPGA has 3 input clocks available to it :

- C16 (Sys_Clk0)

- C21 (Sys_Clk1)

- C02 (Sys_Pclk)

which can be used by the design inside it.

There are 2 clocks inside the FPGA domain, which need to be set **if needed** for WB/PKT FIFO access:

- C40, which provides clock to the WB interface on the AHB2WB bridge connecting the M4 AHB to the FPGA
  if the design needs to expose registers that can be accessed from the M4.

- C41, which provides clock the PKT FIFO interface on the FPGA, if the design wants to use the PKT FIFO
  for communications.

C16 and C21 clocks have a max spec of 72 MHz.

C02 has a max spec of 40 MHz.

For specific designs, there may be limitations on the C16/C21 values, 
which in turn would impose limitations on the HSOSC frequency range.


SPI Slave (with a HOST)
~~~~~~~~~~~~~~~~~~~~~~~

The SPI Slave is clocked from the HOST SPI clock, which is referred to as C00.

The max spec of C00 is 20 MHz.

This clock has a relationship with the AHB Bus Matrix Clock, C10.

C10 must always be greater than (1/3 * C00) for proper operation, which adds to 
the limitation of C10, and hence HSOSC.



Example Design - Calculations
-----------------------------

We will take a walk-through of designing in the clock infrastructure for specific applications, which call
for specific limitations on the peripheral clocks, and hence would affect the HSOSC, and in turn 
other peripheral clocks.

We start with flexible requirements, and as we add more peripherals, we can see how the possible HSOSC
range gets limited.


STEP 1
~~~~~~

We start with a simple application, which needs to only use the UART, and I2C0 to interface a I2C peripheral
to M4.

Let's assume the UART needs to be at 115200 baud, and the I2C peripheral needs to be accessed at 400kHz(max).

UART    <<==     C11     <<==     HSOSC

I2C0    <<==     C08X1   <<==     C08X4   <<==     HSOSC

M4F     <<==     C10     <<==     HSOSC

UART
^^^^

The UART clock is derived from C11 using a fractional divider, so C11 has no really strict constraints,
and can take (almost) any value for achieving 115200 baud.

Example calculation is shown below:

::

  16x_clock = baud * 16
  baud_rate_divider_integer = C11_CLK_FREQ/16x_clock
  baud_rate_divider_float = (float)C11_CLK_FREQ/16x_clock - baud_rate_divider_integer

  IBRD = baud_rate_divider_integer
  FBRD = (int)((baud_rate_divider_float * 64) + 0.5f) -> 64x of float, rounded up to the next integer

  So, given a C11_CLK_FREQ and target baud, after calculations, the actual baud rate would be:
  baud = C11_CLK_FREQ/(IBRD + FBRD/64)*16

  target baud = 115200
  C11_CLK_FREQ = 3072000 Hz

  baud_rate_divider_integer = 3072000/115200*16 = 1
  baud_rate_divider_float = 3072000/115200*16 - IBRD = 1.6667 - 1 = 0.6667
  
  FBRD = int((0.6667*64) + 0.5) = 43
  IBRD = 1

  baud_actual = 3072000/((1 + 43/64)*16) = 3072000/26.75 = 114841.1215

  percentage deviation from target = ((115200 - 114841.1215)/115200) * 100 = 0.311%

  any deviation < +/- 5% is generally acceptable for UART comms, so this is a pretty good value.

I2C0
^^^^

| I2C0 clock is generated using a "pre-scaler" from C08X1.
| Here, it would be worthwhile to note that most of the time exact frequencies of 100kHz, or 400 kHz cannot
  be generated in the system. However, we can get close to these frequencies, and due to the protocol,
  a bit off-value is perfectly acceptable and does not really cause any major problem.

The only aspect to be careful about is not to **exceed** the required frequency, as the I2C peripheral will
not be able to support that.


M4F Core
^^^^^^^^

The application code will determine the performance required out of the M4F core,
which is clocked from C10.

Note that we would prefer to keep the HSOSC values in steps of 1000 KiHz
as far as possible (1024 kHz, or 1024000 Hz), as many of these steps are 
(generally) derivable from 32768 Hz REFCLK.
This reduces the amount of frequencies that we consider in further calculations.

We could always go with lower steps (256 Hz) to determine HSOSC possible values, if required.

So far, note that there are no real constraints on the HSOSC and 
we could choose any frequency suitable to the application.


STEP 2
~~~~~~

Consider that we add an audio use case, and assume use of a PDM mic, also including LPSD.

PDM
^^^

In general, PDM frequencies below are commonly preferred to be used for audio applications:

- 512 kHz
- 768 kHz
- 1.024 MHz
- 1.536 MHz
- 2.4 MHz
- 3.072 MHz
- 4.8 MHz

Note that actual frequencies depend on the specific microphone being used, 
and the oversampling (or decimation ratio), and the above list is representative.

PDM clocks are derived from C30 (max 5 MHz).

1. For example, consider usage of PDM clock at 1.024 MHz (1024000 Hz), then C30 = 1024000 Hz.

   We need to check if this value can be derived from any HSOSC.

   LCM(32768,1024000) = 4096000 Hz (4000 KiHz, 4.096 MHz) which is in range for HSOSC

   HSOSC Constraint Calculation:

   HSOSC = multiple of LCM of (1024000,32768) = multiple of 4096000 Hz (4000 KiHz, 4.096 MHz)

2. Let's consider using PDM clock of 3.072 MHz, then:

   LCM (3072000,32768) = 12288000 Hz (12000 KiHz, 12.288 MHz) which is in range for HSOSC

   HSOSC Constraint Calculation:

   HSOSC = multiple of LCM of (3072000,32768) = multiple of 12288000 Hz (12000 KiHz, 12.288 MHz)

3. Now, consider using PDM clock of 4.8 MHz.

   LCM(32768,4800000) = 307200000 Hz (307.2 MHz) which is **NOT** in range for HSOSC
   
   Hence, we cannot obtain a PDM clock of exactly 4.8 MHz in the system.


LPSD
^^^^

LPSD clocks are derived from C31 which has a max spec of 1 MHz.

It is preferable to use 512 KHz for LPSD clock.

LCM (32768, 512000) = 4096000 Hz (4000 KiHz, 4.096 MHz) which is in range for HSOSC.

It is then possible to get 512000 Hz.

HSOSC Constraint Calculation, assuming PDM Clock at 1024000 Hz:

HSOSC = multiple of (LCM of 512000, 1024000, 32768) = multiple of 4096000 Hz (4000 KiHz, 4.096 MHz)

which is same as with the PDM constraint alone, so the LPSD clock is also possible to achieve.

Similarly, we can see that we get the same constraint as above of 12288000 Hz 
with PDM clock of 3.072 MHz as well.

Consider that we finalized the PDM Clock at 1024000 Hz and LPSD at 512000 Hz at this point.


STEP 3
~~~~~~

Consider that we add SPI1 Master in the design, which communicates with a
SPI Slave device (say a display), which has a max frequency spec of 6 MHz (6000000 Hz)

SPI1 Master
^^^^^^^^^^^

SPI1M is derived from C02 and needs a minimum div of 2 or greater, even divider values only (2,4,6,8 ...).

So far, the constraint on HSOSC is to be a multiple of 4096000 Hz.

With simple variations in HSOSC to get unique SPI1M clocks, we can see that:

- | HSOSC = 4096000 Hz
  | C02 DIV = 1
  | SP1M BAUD DIV = 2 (minimum)
  | SPI1M Frequency = 4096000 Hz (low)

- | HSOSC = 20480000 Hz
  | C02 DIV = 1
  | SP1M BAUD DIV = 4
  | SPI1M Frequency = 5120000 Hz (ok) 

Note that, we would not be able to take advantage of the top speed available from the 
device point of view, because we are constrained by the system design.

We can arrive at, with a bit more variation in the C02 DIV and HSOSC values at the following combo:

- | HSOSC = 73728000 Hz (72000 KiHz)
  | C02 DIV = 7
  | C02 Frequency = 73728000 / 7 Hz = 10532571.4286 Hz (approx 10.5 MHz)
  | SP1M BAUD DIV = 2 (minimum)
  | SPI1M Frequency = 10532571.4286/2 = 5266285.71428 Hz (approx 5.266 MHz)

We can see that we can arrive at an even better SPI frequency of 5.266 MHz with some variations.

In this particular case, the downside is needing the HSOSC to go to 72000 KiHz, which is not great for power
consumption, but in situations where high performance is required, this might be an acceptable solution.

:code:`TODO: check if we can make such calculations possible automatically, maybe with a tool/utility.`

Note that we have not really added a constraint on the HSOSC here, as SPI (like I2C) can run at
any frequency below the max spec of the devices, faster the better, but slower is acceptable as 
long as the application is ok. 

This is highly dependent on the criticality of the SPI transactions.


STEP 4
~~~~~~

Consider adding the FPGA into the usage, specifically using the FPGA UART design.

FPGA UART
^^^^^^^^^

The FPGA UART core has integer divider only, and uses C21 to generate the UART clock,
and requires C21 at a multiple of 1.8432 MHz to be able to achieve standard baud rates (115200, 9600 etc.)

In general (integer div), UART Clock = baudrate x 16, and hence sometimes called the :code:`16X clock`.

With C21 having a constraint of being a multiple of 1.8432 MHz, it follows that
the HSOSC needs to be a multiple of 1.8432 MHz too.

LCM (32768, 1843200) = 7372800 Hz (7200 KiHz, 7.372 MHz) which is in range for HSOSC.

So far, the constraint on the HSOSC is to be a multiple of 4096000 Hz, and adding 1843200 Hz to this,

HSOSC = multiple of (LCM of 4096000, 1843200) = multiple of 36864000 Hz (36000 KiHz, 36.864 MHz)

Side Note: we skipped using some of the constraints (LPSD 512000 Hz and REFCLK 32768 Hz) as they have already
been factored in the previous steps. It can be verified that even including both of these, the LCM 
obtained is the same as above (36864000 Hz)

As a consequence, HSOSC can only have 2 values:

- 36864000 Hz (36000 KiHz, 36.864 MHz)
- 73728000 Hz (72000 KiHz, 73.728 MHz)

as the next multiple exceeds the max HSOSC spec of 80 MHz.

At this point, we can see that if we are going to use the FPGA UART design, the HSOSC values are pretty
restricted, and can run only at 36.864 MHz and 73.728 MHz.


.. |PRE RELEASE| image:: https://img.shields.io/static/v1?label=STATUS&message=PRE-RELEASE&color=yellow&style=for-the-badge
   :target: none

.. |WORK IN PROGRESS| image:: https://img.shields.io/static/v1?label=STATUS&message=WORK-IN-PROGRESS&color=red&style=for-the-badge
   :target: none