/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah
      zoltan_padrah@users.sourceforge.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KTLCONFIG_H
#define KTLCONFIG_H

#include <QObject>


class KtlConfig : public QObject
{
    Q_OBJECT

public:
    /**
     \return the (single) instance of KtlConfig
     */
    static KtlConfig *self();

    /**
     delete the instance of KtlConfig. Use only when exiting the program
     (to make Valgrind happy)
     */
    static void destroy();

    // asm formatter

    #define INDENT_INT_OPTION(name) \
    public: \
        int indent##name() const { \
            return m_indent##name; \
        } \
        void setIndent##name(int value){    \
            m_indent##name;  \
    }   \
    private:    \
        bool m_indent##name ;

    INDENT_INT_OPTION(AsmName);
    INDENT_INT_OPTION(AsmData);
    INDENT_INT_OPTION(Comment);
    INDENT_INT_OPTION(Equ);
    INDENT_INT_OPTION(EquValue);

    #undef INDENT_INT_OPTION

public:

    bool autoFormatMBOoutput() const {
        return m_autoFormatMbOutput;
    }
    void setAutoFormatMBOutput(bool value){
        m_autoFormatMbOutput = value;
    }

    // logic

    /** Rising Trigger Threshold */
    double logicRisingTrigger() const { return m_logicRisingTrigger; }
    void setLogicRisingTrigger(double value) {
        m_logicRisingTrigger = value;
    }

    /** Falling Trigger Threshold */
    double logicFallingTrigger() const {
        return m_logicFallingTrigger;
    }
    void setLogicFallingTrigger(double value){
        m_logicFallingTrigger = value;
    }

    /** Logic Output High */
    double logicOutputHigh() const {
        return m_LogicOutputHigh;
    }
    void setLogicOutputHigh(double value){
        m_LogicOutputHigh = value;
    }

    /** Logic Output High Impedance*/
    double logicOutputHighImpedance() const {
        return m_LogicOutputHighImpedance;
    }
    void setLogicOutputHighImpedance(double value){
        m_LogicOutputHighImpedance = value;
    }

    /** Logic Output Low Impedance*/
    double logicOutputLowImpedance() const {
        return m_LogicOutputLowImpedance;
    }
    void setLogicOutputLowImpedance(double value){
        m_LogicOutputLowImpedance = value;
    }

    enum HexFormat {
        inhx32,
        inhx8m,
        inhx8s,
        inhx16
    };

    /// gpasm hex format
    HexFormat hexFormat() const {
        return m_hexFormat;
    }
    void setHexFormat(HexFormat value){
        m_hexFormat = value;
    }

    enum EnumRadix {
        Decimal,
        Binary,
        Octal,
        Hexadecimal
    };

    /// gpasm radix
    EnumRadix radix() const {
        return m_radix;
    }
    void setRadix(EnumRadix value){
        m_radix = value;
    }

    enum EnumGpasmWarningLevel {
        All,
        Warnings,
        Errors
    };

    /// gpasm warning level
    EnumGpasmWarningLevel gpasmWarningLevel() const {
        return m_gpasmWarningLevel;
    }
    void setGpasmWarningLevel(EnumGpasmWarningLevel value) {
        m_gpasmWarningLevel = value;
    }

    /// gpasm ignore case
    bool ignoreCase() const {
        return m_ignoreCase;
    }
    void setIgnoreCase(bool value) {
        m_ignoreCase = value;
    }

    /// gpasm dos format for ASM
    bool dosFormat() const {
        return m_dosFormat;
    }
    void setDosFormat(bool value){
        m_dosFormat = value;
    }

    /// other gpasm options
    QString miscGpasmOptions() const {
        return m_miscGpasmOptions;
    }
    void setMischGpasmOptions(QString value){
        m_miscGpasmOptions = value;
    }

    // SDCC related
    #define SDCC_BOOL_OPTION(name) \
    public: \
        bool sDCC_##name() const { \
            return m_sdcc_##name; \
        } \
        void setSDCC_##name(bool value){    \
            m_sdcc_##name;  \
    }   \
    private:    \
        bool m_sdcc_##name ;

    ///Don't search in the standard library directory
    SDCC_BOOL_OPTION(nostdlib);
    /// Don't search in the standard include director
    SDCC_BOOL_OPTION(nostdinc);
    /// Disable pedantic warnings
    SDCC_BOOL_OPTION(less_pendatic);
    /// Strictly follow the C89 standard
    SDCC_BOOL_OPTION(std_c89);
    /// Strictly follow the C99 standard
    SDCC_BOOL_OPTION(std_c99);
    /// Stack automatic variables
    SDCC_BOOL_OPTION(stack_auto);
    /// Integer libraries have been compiled as reentrant
    SDCC_BOOL_OPTION(int_long_reent);
    /// Floating point library is has been compiled as reentrant
    SDCC_BOOL_OPTION(float_reent);
    /// Leave out the frame pointer
    SDCC_BOOL_OPTION(fommit_frame_pointer);
    /// Don't memcpy initialized data from code space to xdata space
    SDCC_BOOL_OPTION(no_xinit_opt);
    /// Callee will always save registers used
    SDCC_BOOL_OPTION(all_callee_saves);
    /// Don't overlay parameters and local variables
    SDCC_BOOL_OPTION(nooverlay);
    /// Disable the GCSE optimization
    SDCC_BOOL_OPTION(nogcse);
    /// Don't optimize labels
    SDCC_BOOL_OPTION(nolabelopt);
    /// Disable optimization of invariants
    SDCC_BOOL_OPTION(noinvariant);
    /// Disable loop variable induction
    SDCC_BOOL_OPTION(noinduction);
    /// Disable peep-hole optimization
    SDCC_BOOL_OPTION(no_peep);
    /// Don't do loop reversal optimization
    SDCC_BOOL_OPTION(noloopreverse);
    /// Optimize for compact code
    SDCC_BOOL_OPTION(opt_code_size);
    /// Optimize for fast code
    SDCC_BOOL_OPTION(opt_code_speed);
    /// Pass inline assembler code through peep hole optimizer
    SDCC_BOOL_OPTION(peep_asm);
    /// Don't generate boundary check for jump tables
    SDCC_BOOL_OPTION(nojtbound);
    /// Don't use default libraries
    SDCC_BOOL_OPTION(nodefaultlibs);
    /// Don't generate BANKSEL directives
    SDCC_BOOL_OPTION(pno_banksel);
    /// Use large stack model
    SDCC_BOOL_OPTION(pstack_model_large);
    /// Show more debug info in assembly output
    SDCC_BOOL_OPTION(debug_xtra);
    /// Explicit enable of peephole
    SDCC_BOOL_OPTION(denable_peeps);
    /// Dump call tree in .calltree file
    SDCC_BOOL_OPTION(calltre);
    /// Enable stack optimizations
    SDCC_BOOL_OPTION(fstack);
    /// Try to use conditional BRA instead of GOTO
    SDCC_BOOL_OPTION(optimize_goto);
    /// Try to optimize some compares
    SDCC_BOOL_OPTION(optimize_cmp);
    /// Thorough data flow analysis
    SDCC_BOOL_OPTION(optimize_df);

    #undef SDCC_BOOL_OPTION

public:

    /// Other Options for SDCC
    QString miscSDCCOptions() const
        { return m_miscSDCCOptions; }
    void setMiscSDCCOptions(QString value) {
        m_miscSDCCOptions = value;
    }

    /// The application to use to program the PIC.
    QString picProgrammerProgram() const
        { return m_picProgrammerProgram; }
    void picProgrammerProgram(QString value){
        m_picProgrammerProgram = value;
    }

    /// The device (serial, parallel, etc...) used to program the PIC.
    QString picProgrammerPort() const
        { return m_picProgrammerPort; }
    void picProgrammerPort(QString value){
        m_picProgrammerPort = value;
    }

    /// Check this if SDCC uses shared libs for compiling (newer SDCC does so)
    bool gplink_link_shared() const {
        return m_gplink_link_shared;
    }
    void set_gplink_link_shared(bool value){
        m_gplink_link_shared = value;
    }

private:
    KtlConfig(QObject* parent = 0);
    virtual ~KtlConfig();

    static KtlConfig *m_self;

    bool m_autoFormatMbOutput;

    double m_logicRisingTrigger;
    double m_logicFallingTrigger;
    double m_LogicOutputHigh;
    double m_LogicOutputHighImpedance;
    double m_LogicOutputLowImpedance;
    HexFormat m_hexFormat;
    EnumRadix m_radix;
    EnumGpasmWarningLevel m_gpasmWarningLevel;
    bool m_ignoreCase;
    bool m_dosFormat;
    QString m_miscGpasmOptions;
    QString m_miscSDCCOptions;
    QString m_picProgrammerProgram;
    QString m_picProgrammerPort;
    bool m_gplink_link_shared;
};

#endif // KTLCONFIG_H
