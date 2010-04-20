/*
    ISimulationManager interface.
        A controller for simuations in ktechlab

    // TODO insert GPL header
*/

namespace KTechLab {

    /**
     * A simulation manager interface. 
     * This interface provides access to all the simulation-related
     * services.
     */
    class ISimulationManager {
        public:
            /** @return the static ISimulationManager instance */
            static ISimulationManager *self();

            // ISimulator *createSimulatorForDocument(IDocument *document)
        protected:
          ISimulationManager();

          ISimulationManager *m_self;
    };

}
