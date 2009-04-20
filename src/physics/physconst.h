/**
	Physical Constants
*/

#ifndef PHYSCONST_H
#define PHYSCONST_H

#include <cmath>

/**
	Boltzmann's constant  (J/K)
wikipedia quote:
	The Boltzmann constant (k or kB) is the physical constant relating energy at the particle level with temperature observed at the bulk level. It is the gas constant R divided by the Avogadro constant N_A:

    k = R / N_A

	It has the same units as entropy. It is named after the Austrian physicist Ludwig Boltzmann

Uncertainty: 0.000 0024 x 10^(-23) J K^(-1)

Information from: 2006 CODATA
*/
#define BOLTZMANN_CONSTANT 1.3806504e-23

/**
	Planck's Constant  (joule-second)

The Planck constant (denoted h), also called Planck's constant, is a physical constant used to describe the sizes of quanta in quantum mechanics. It is named after Max Planck, one of the founders of quantum theory. The Planck constant is the proportionality constant between energy (E) of a photon and the frequency of its associated electromagnetic wave (?). This relation between the energy and frequency is called the Planck relation.

    E = h\nu\,

A closely related constant is the reduced Planck constant, denoted ? ("h-bar"), which is equal to the Planck constant divided by (or reduced by) 2?.

    \hbar = \frac{h}{2 \pi}\,

The reduced Planck constant is used when frequency is expressed in terms of radians per second instead of cycles per second. The expression of a frequency in radians per second is often called angular frequency (?), where ? = 2??.

    E = \hbar\omega\,

Unicode reserves codepoints U+210E (?) for the Planck constant, and U+210F (?) for the reduced Planck constant.

Planck hypothesized (correctly, as it later turned out) that some types of energy could not take on any indiscriminate value: instead, the energy must be some multiple of a very small quantity (later to be named a "quantum"). This is counterintuitive in the everyday world, where it is possible to "make things a little bit hotter" or "move things a little bit faster", because the quanta of energy are very, very small in comparison to everyday human experience. Nevertheless, it is impossible, as Planck found out, to explain some phenomena without accepting that energy is discrete: that is to say like the integers 1, 2, 3? instead of the line of all possible numbers.

Uncertainty: 0.000 000 33 x 10-34 J s
*/
#define PLANCK_CONSTANT 6.62606896e-34

/**
	Elementary charge (columbs)
wikipedia quote:
	The elementary charge, usually denoted e,[1] is the electric charge carried
 by a single proton, or equivalently, the negative of the electric charge carried by
 a single electron. This is a fundamental physical constant.

It has a measured value of approximately 1.602 176 487(40) × 10?19 coulombs[2]. In cgs, the value is 4.803 204 27(12) × 10?10 statcoulombs[3]. In the system of atomic units as well as some other systems of natural units, e functions as the unit of electric charge, i.e. e = 1 in those unit systems.

The magnitude of the elementary charge was first measured in Robert Millikan's famous oil-drop experiment in 1909.

Charge quantization is the statement that every particle or object has a charge which is an integer multiple of the elementary charge e: A charge can be exactly 0, or exactly e, -e, 2e, etc., but not, say, half of e, or -3.8 times e, etc. (This statement must be interpreted carefully; see below for more details.)

This is the reason for the terminology "elementary charge": It is meant to imply that it is an indivisible unit of charge.

Uncertainty: 0.000000040 x 10-19 C

Information from: 2006 CODATA
*/
#define ELEMENTARY_CHARGE 1.602176487e-19

/**
	Electron mass, (kg)

The mass of ye olde electron. 

Why do we multiply a gram by a thousand and then divide it by 10^31 in this case? sheesh!

Uncertainty: 0.00000045 x 10-31 kg
*/
#define ELECTRON_MASS 9.10938215e-31


/** Speed of Light (Meter/SECOND)

Defined;
We force it to floating point here to make sure there isn't an overflow error when squaring it.
*/
#define LIGHTSPEED  299792458.0

/**
Vacuum permeability  (N/A^2)

The vacuum permeability, referred to by international standards organizations as the magnetic constant,[1][2] and denoted by the symbol ?0 (also called the permeability of free space and of empty space), is a fundamental physical constant, relating mechanical and electromagnetic units of measurement. In the International System of Units (SI), its value is defined in free space (not measured see defined variables, Table 1 in CODATA report[3]) as:

    \mu_0 \ \overset{\underset{\mathrm{def}}{}}{=}\ 4 \pi\ \times \ 10^{-7}\ N/A2 = 4?×10?7 H/m, or approximately 1.2566×10?6 H/m (or T·m/A).[1]

This value is a consequence of the definition of the ampere in terms of the force per unit length between infinitely long parallel wires of zero cross-section in vacuum.[4] For a discussion of this force, see Serway and Jewett,[5] or Monk.[6] See also Ampère's force law.

At present, techniques to establish the realization of an ampere have a relative uncertainty of approximately a few parts in 107, and involve realizations of the watt, the ohm and the volt.[7]

In vacuum, the magnetic constant defines the value of the magnetic field strength, or H-field (calculated from currents) in terms of the magnetic flux density, or B-field (for calculating Lorentz force):[8]

    \mathbf{B} = \mu_0 \ \mathbf{H}.

See Maxwell's equations.

The magnetic constant ?0 is related to two other defined constants, the electric constant ?0 and the speed of light in vacuum c0 according to the identity:[9]

    {c_0}^2 \, \epsilon_0 \, \mu_0 = 1. 
*/
#define MAGNETIC_CONSTANT (4.0e-7 * M_PI)

/**
	Permittivity of Free Space (Farad/Meter)

Wikipedia quote: 
Permittivity is a physical quantity that describes how an electric field affects, and is affected by a dielectric medium, and is determined by the ability of a material to polarize in response to the field, and thereby reduce the total electric field inside the material. Thus, permittivity relates to a material's ability to transmit (or "permit") an electric field.

It is directly related to electric susceptibility. For example, in a capacitor, an increased permittivity allows the same charge to be stored with a smaller electric field (and thus a smaller voltage), leading to an increased capacitance.

In SI units, permittivity is measured in farads per meter (F/m). The constant value ?0 is known as the electric constant or the permittivity of free space, and has the value ?0 ? 8.854 187 817? × 10?12 F/m or A2s4 kg-1m?3 in SI base units; (the ellipsis (?) indicates termination of a nonrecurring decimal, not an experimental uncertainty).

	defined: 1/(magnetic constant * (speed of light)^2)

*/

// TODO: check to minimize computational drift. 
#define BASE_PERMITTIVITY (1 / (MAGNETIC_CONSTANT * LIGHTSPEED * LIGHTSPEED))


#endif
