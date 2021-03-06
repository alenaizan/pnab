/**@file
 * @brief A file for declaring various classes for defining options
 */

#ifndef PNAB_CONTAINERS_H
#define PNAB_CONTAINERS_H

#include <string>
#include <random>
#include <array>
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/obconversion.h>
#include <openbabel/math/matrix3x3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180.0)
#endif

//! The PNAB name space contains all the C++ classes and functions for the proto-Nucleic Acid Builder.
namespace PNAB {
    /**
    * @brief A class for holding necessary and optional runtime parameters for conformational searches
    *
    * The runtime parameters are used for building the strands and during the conformational search.
    *
    * @sa Chain
    * @sa ConformationSearch
    */
    class RuntimeParameters {

    public:
        /**
        * @brief Empty constructor.
        *
        * This empty constructor can be used. After that, values for the member variables should be specified.
        */
        RuntimeParameters() : energy_filter{}, max_distance(), ff_type(), glycosidic_bond_distance(0.0),
                              num_steps(0), seed(0), weighting_temperature(298.0), monte_carlo_temperature(298.0),
                              mutation_rate(0.75), crossover_rate(0.75), population_size(1000), strand{}, is_hexad(false),
                              build_strand(std::vector<bool> {true, false, false, false, false, false}), num_candidates(10),
                              strand_orientation(std::vector<bool> {true, true, true, true, true, true}){};

        // Thresholds
        std::vector<double> energy_filter;      /*!< @brief [max bond E, max angle E, max torsion E, max VDW E, max total E]
                                                *
                                                * - Maximum accepted energy for newly formed bonds in the backbone (kcal/mol/bond)
                                                * - Maximum accepted energy for newly formed angles in the backbone (kcal/mol/angle)
                                                * - Maximum accepted torsional energy for rotatable bonds (kcal/mol/nucleotide)
                                                * - Maximum accepted van der Waals energy (kcal/mol/nucleotide)
                                                * - Maximum accepted total energy (kcal/mol/nucleotide)
                                                *
                                                * @sa Chain::generateConformerData
                                                * @sa Chain::fillConformerEnergyData
                                                */

        double max_distance;                    /*!< @brief Maximum accepted distance (Angstrom) between head and tail of successive nucleotides
                                                *
                                                * This distance is used to quickly screen rotamers. The algorithm searches for backbone
                                                * candidates that are periodic in terms of the helical structure. Thus, the terminal atoms
                                                * in adjacent candidates must be within a small distance to allow for a bond to form.
                                                * This distance is recommended to be less than 0.1 Angstroms. The extra terminal atom
                                                * in the adjacent nucleotide is then removed.
                                                *
                                                * @sa ConformationSearch::measureDistance
                                                */

        // Force Field Parameter
        std::string ff_type;                    /*!< @brief The type of the forcefield such as "GAFF" or "MMFF94"; available through Openbabel
                                                *
                                                * @sa Chain::Chain
                                                */

        // Algorithm parameters
        std::string search_algorithm;           /*!< @brief The search algorithm
                                                *
                                                * There are six search algorithms:
                                                * - Systematic search
                                                * - Monte Carlo search
                                                * - Weighted Monte Carlo search
                                                * - Random search
                                                * - Weighted random search
                                                * - Genetic algorithm search
                                                *
                                                *   @sa ConformationSearch
                                                */

        std::size_t num_steps;                  /*!< @brief The number of points sampled in Monte Carlo and random searches
                                                * and the number of generations in the genetic algorithm search
                                                *
                                                * @sa ConformationSearch::MonteCarloSearch
                                                * @sa ConformationSearch::RandomSearch
                                                * @sa ConformationSearch::GeneticAlgorithmSearch
                                                */
        unsigned int seed;                      /*< @brief The seed for the random number generator. Use the same value to get reproducible
                                                * results.
                                                *
                                                *  @sa ConformationSearch::MonteCarloSearch
                                                *  @sa ConformationSearch::RandomSearch
                                                *  @sa ConformationSearch::GeneticAlgorithmSearch
                                                */

        double dihedral_step;                   /*!< @brief The dihedral step size for systematic search (degrees)
                                                *
                                                * The number of steps will be \f$(\frac{360.0}{\textrm{dihedral step}})^{\textrm{number of rotatable dihedrals}}\f$.
                                                *
                                                * @sa ConformationSearch::SystematicSearch
                                                */

        double weighting_temperature;           /*!< @brief The temperature used to compute the weighted probability for weighted
                                                * Monte Carlo and weighted random searches
                                                *
                                                * @sa ConformationSearch::WeightedDistributions
                                                */

        double monte_carlo_temperature;         /*!< @brief The temperature used in the Monte Carlo acceptance and rejection procedure
                                                *
                                                * @sa ConformationSearch::MonteCarloSearch
                                                */

        double mutation_rate;                   /*!< @brief The mutation rate in the genetic algorithm search
                                                *
                                                * @sa ConformationSearch::GeneticAlgorithmSearch
                                                */

        double crossover_rate;                  /*!< @brief The crossover rate in the genetic algorithm search
                                                *
                                                * @sa ConformationSearch::GeneticAlgorithmSearch
                                                */

        int population_size;                    /*!< @brief The population size in the genetic algorithm search
                                                *
                                                * @sa ConformationSearch::GeneticAlgorithmSearch
                                                */

        //Strand parameters
        std::vector<std::string> strand;        //!< @brief The names of each base used in the strand
        std::vector<bool> build_strand;         /*!< @brief Defines whether to build a given strand
                                                *
                                                * @sa Chain::Chain
                                                */
        std::vector<bool> strand_orientation;   /*!< @brief Defines strand orientation for each strand in the hexad
                                                *
                                                * @sa Chain::setCoordsForChain
                                                * @sa Chain::setCoordsForChain
                                                */
        bool is_hexad;                          //!< @brief Defines whether the 60 degrees rotation for hexads is performed
        // Glycosidic bond
        double glycosidic_bond_distance;        //!< @brief Set a user-defined glycosidic bond distance (in Angstroms). If zero (default), sets the distance based on van der Waals radii
        unsigned int num_candidates;            //!< @brief Quit after finding the specified number of accepted candidates
    };

    /**
    * @brief A class for holding values for all helical parameters
    *
    * The helical parameters are used for generating the geometries of the nucleobases in the strands.
    * This class holds the value for six helical parameters (helical twist, inclination, tip, helical rise, x-displacement, and y-displacement).
    * It can also compute the helical parameters given the step parameters (twist, roll, tilt, rise, slide, shift).
    * Internally, when the step parameters are provided, the helical parameters are computed. The helical parameters are used for
    * generating the geometries. This class also implements the base-pair parameters (buckle, propeller, opening, shear, stretch, and stagger).
    * The class holds functions to generate the geometries.
    *
    * @sa Chain::setCoordsForChain
    * @sa ConformationSearch::measureDistance
    */
    class HelicalParameters {

    public:
        /**
        * @brief Empty constructor
        *
        * This empty constructor can be used. After that, values for the member variables should be specified.
        */
        HelicalParameters() : h_twist{0}, h_rise{0}, inclination{0}, tip{0}, x_displacement{0}, y_displacement{0},
                              shift{0}, slide{0}, rise{0}, tilt{0}, roll{0}, twist{0},
                              buckle{0}, propeller{0}, opening{0}, shear{0}, stretch{0}, stagger{0}, is_helical{true} {};

        //Helical parameters
        double     inclination,                         //!< @brief Inclination
                   tip,                                 //!< @brief Tip
                   h_twist,                             //!< @brief Helical twist
                   x_displacement,                      //!< @brief X-Displacement
                   y_displacement,                      //!< @brief Y-Displacement
                   h_rise,                              //!< @brief Helical rise
                   shift,                               //!< @brief Shift
                   slide,                               //!< @brief Slide
                   rise,                                //!< @brief Rise
                   tilt,                                //!< @brief Tilt
                   roll,                                //!< @brief Roll
                   twist,                               //!< @brief Twist
                   buckle,                              //!< @brief Buckle
                   propeller,                           //!< @brief Propeller twist
                   opening,                             //!< @brief Opening
                   shear,                               //!< @brief Shear
                   stretch,                             //!< @brief Stretch
                   stagger;                             //!< @brief Stagger
        bool is_helical;                                //!< @brief Are the base parameters helical or step parameters

        /**
        * @brief A function to compute the helical parameters. This should be called when the the step parameters are provided.
        *
        * @sa is_helical
        * @sa StepParametersToReferenceFrame
        * @sa ReferenceFrameToHelicalParameters 
        */
        void computeHelicalParameters();

        /**
        * @brief Get the global translation vector 
        * 
        * For the base step transformation, the HelicalParameters::x_displacement and HelicalParameters::y_displacement are used.
        * For the base-pair transformation, the HelicalParameters::shear and HelicalParameters::stretch are used.
        * The base pair parameters are divided by two and set to positive or negative values depending on which strand is being constructed.
        *
        * @param is_base_pair Whetehr the vector requested is for base-pair transformation
        * @param is_second_strand Whether the strand being constructed is the second strand. Used only for base-pair parameters
        *
        * @returns The translation vector
        */
        OpenBabel::vector3 getGlobalTranslationVec(bool is_base_pair=false, bool is_second_strand=false);

        /**
        * @brief Get the step translation vector
        *
        * For the base step transformation, the HelicalParameters::h_rise is used.
        * For the base-pair transformation, the HelicalParameters::stagger is used.
        * The base pair parameters are divided by two and set to positive or negative values depending on which strand is being constructed.
        *
        * @param n The sequence of the nucleobase in the strand. Used only for the base step transformation
        * @param is_base_pair Whetehr the vector requested is for base-pair transformation
        * @param is_second_strand Whether the strand being constructed is the second strand. Used only for base-pair parameters
        *
        * @returns The step translation vector
        */
        OpenBabel::vector3 getStepTranslationVec(unsigned n = 0, bool is_base_pair=false, bool is_second_strand=false);

        /**
        * @brief Get the global rotation matrix
        *
        * For the base step transformation, the HelicalParameters::tip and HelicalParameters::inclination are used.
        * For the base-pair transformation, the HelicalParameters::buckle and HelicalParameters::propeller are used.
        * The base pair parameters are divided by two and set to positive or negative values depending on which strand is being constructed.
        *
        * @param is_base_pair Whetehr the vector requested is for base-pair transformation
        * @param is_second_strand Whether the strand being constructed is the second strand. Used only for base-pair parameters
        *
        * @returns The global rotation matrix
        *
        * @sa rodrigues_formula
        */

        // Lu, X. J., El Hassan, M. A., & Hunter, C. A. (1997). Structure and conformation of helical nucleic acids:
        // rebuilding program (SCHNArP). Journal of molecular biology, 273(3), 681-691.
        OpenBabel::matrix3x3 getGlobalRotationMatrix(bool is_base_pair=false, bool is_second_strand=false);

        /**
        * @brief Get the step rotation matrix
        * 
        * For the base step transformation, the HelicalParameters::twist is used.
        * For the base-pair transformation, the HelicalParameters::opening is used.
        * The base pair parameters are divided by two and set to positive or negative values depending on which strand is being constructed.
        *
        * @param n The sequence of the nucleobase in the strand. Used only for the base step transformation
        * @param is_base_pair Whetehr the vector requested is for base-pair transformation
        * @param is_second_strand Whether the strand being constructed is the second strand. Used only for base-pair parameters
        *
        * @returns The step rotation matrix
        */
        OpenBabel::matrix3x3 getStepRotationMatrix(unsigned n = 0, bool is_base_pair=false, bool is_second_strand=false);

    private:
        /**
        * @brief Rodrigues rotation formula for rotating a vector in space
        *
        * Outputs a 3x3 rotation matrix.
        *
        * @param axis_vector A unit vector defining the axis about which to rotate by angle theta
        * @param theta The angle at which to rotate about vector given by axis
        *
        * @return The new rotation matrix
        */
        OpenBabel::matrix3x3 rodrigues_formula(OpenBabel::vector3 axis_vector, double theta);

        /**
        * @brief Computes the origin and direction vectors given a set of step parameters
        *
        * Outputs a list containing the new origin, and x, y, and z direction vectors. It uses the class member variables
        * to access the step parameters.
        *
        * Reference: El Hassan, M. A., and C. R. Calladine. "The assessment of the geometry of dinucleotide steps
        * in double-helical DNA; a new local calculation scheme." Journal of molecular biology 251.5 (1995): 648-664.
        *
        * @return The origin and direction vectors
        *
        * @sa ReferenceFrameToHelicalParameters
        */ 
        std::vector<OpenBabel::vector3> StepParametersToReferenceFrame();
 
        /**
        * @brief Computes the helical parameters given the origin and direction vectors of the second base
        *
        * This function computes the helical parameters and set the values for the appropriate member variables
        *
        * Reference: Lu, Xiang-Jun, M. A. El Hassan, and C. A. Hunter. "Structure and conformation of helical nucleic acids:
        * analysis program (SCHNAaP)." Journal of molecular biology 273.3 (1997): 668-680.
        *
        * @param origin2 The coordinates of the origin of the second base pair
        * @param x2 The x direction vector for the second base pair
        * @param y2 The y direction vector for the second base pair
        * @param z2 The z direction vector for the second base pair
        *
        * @sa StepParametersToReferenceFrame
        */  
        void ReferenceFrameToHelicalParameters(OpenBabel::vector3 origin2, OpenBabel::vector3 x2, OpenBabel::vector3 y2, OpenBabel::vector3 z2);

    };

    /**
    * @brief Class for holding backbone information
    *
    * The backbone here refers to the backbone in a single nucleotide. This class holds information
    * on the molecular structure of the backbone and the bonds that the backbone form with the
    * nucleobases and the adjacent backbones. This class also has functions to manipulate the backbone.
    *
    * @sa BaseUnit
    * @sa ConformationSearch
    */
    class Backbone {
    public:

        /**
        * @brief Empty constructor
        *
        * This empty constructor can be used. After that, values for the member variables should be specified.
        */
        Backbone() : backbone{}, file_path{}, interconnects{}, linker{}, vector_atom_deleted{}, fixed_bonds{} {}

        /**
        * @brief Constructor for the backbone unit
        *
        * @param file_path The path to the file containing the molecule.
        * @param interconnects The atom indices that define the periodic conditions between backbones { head, tail }.
        * @param linker The atom indices used to align and connect backbone to base in the nucleotide.
        * @param fixed_bonds A vector containing pairs of indices defining fixed rotatable bonds during dihedral search.
        */
        Backbone(std::string file_path, std::array<unsigned, 2> interconnects, std::array<unsigned,2> linker, std::vector<std::vector<unsigned>> fixed_bonds = {});

        /**
        * @brief Gives the pointer to an atom that is the head from Backbone::interconnects{head, tail}
        * @return The atom pointer from the backbone OBMol object
        */
        OpenBabel::OBAtom* getHead() {
            return backbone.GetAtom(interconnects[0]);
        }

        /**
        * @brief Gives the pointer to an atom that is the tail from Backbone::interconnects{head, tail}
        * @return Pointer to the atom for the tail
        */
        OpenBabel::OBAtom* getTail() {
            return backbone.GetAtom(interconnects[1]);
        }

        /**
        * @brief Get the first Backbone::linker atom pointer
        * @return Pointer to the atom that is the one linking to the Base
        */
        OpenBabel::OBAtom* getLinker() {
            return backbone.GetAtom(linker[0]);
        }

        /**
        * @brief Get the second Backbone::linker atom pointer (which is probably a hydrogen)
        * @return Pointer to the atom that defines the vector from the backbone to the base
        */
        OpenBabel::OBAtom* getVector() {
            if (!vector_atom_deleted)
                return backbone.GetAtom(linker[1]);
            else {
                std::cerr << "Called getVector() for backbone with no vector atom." << std::endl;
                return nullptr;
            }
        }

        /**
        * @brief Centers the molecule. Basically just an alias of the Center() function from OpenBabel.
        */
        void center() {
            backbone.Center();
        }

        /**
        * @brief Rotates the molecule by a matrix. Basically just an alias of the Rotate() function from OpenBabel.
        * @param rot The matrix by which to rotate the molecule
        */
        void rotate(double* rot) {
            backbone.Rotate(rot);
        }

        /**
        * @brief Translates the molecule by a vector. Basically just an alias of the Translate() function from OpenBabel.
        * @param vec The vector by which to translate the molecule
        */
        void translate(OpenBabel::vector3 vec) {
            backbone.Translate(vec);
        }

        /**
        * @brief Gives a copy of the molecule in the backbone, Backbone::backbone
        * @return A copy of the backbone molecule
        */
        OpenBabel::OBMol getMolecule() {
            return backbone;
        }

        /**
        * @brief Deletes the atom from getVector() safely. If the atom is already deleted, nothing happens.
        */
        void deleteVectorAtom();

        std::array<unsigned , 2> interconnects,          //!< @brief The atom indices that define the periodic conditions between backbones { head, tail }
                                 linker;                 //!< @brief The atom indices used to align and connect backbone to base in the nucleotide
        std::vector<std::vector<unsigned>> fixed_bonds;  //!< @brief A vector containing pairs of indices defining fixed rotatable bonds during dihedral search
        OpenBabel::OBMol backbone;                       //!< @brief The molecule for the backbone
        std::string file_path;                           //!< @brief The path to the file containing the molecule

    private:
       /**
       * @brief Does some basic sanity checks (such as whether or not the indices of the atom are within the range
       * of the molecule)
       */
        void validate();

        bool vector_atom_deleted;                        //!< @brief Whether or not the atom from getVector() has been deleted
    };

    /**
    * @brief Class to fully define bases (i.e. Adenine, Cytosine)
    *
    * This class holds information on the molecular structure of one nucleobase
    * and the bond that it should form with the backbone. This class also has functions
    * to manipulate the nucleobase and gets information about it.
    *
    * @sa Bases
    * @sa BaseUnit
    */
    class Base {

    public:

        /**
        * @brief Empty constructor
        *
        * This empty constructor can be used. After that, values for the member variables should be specified.
        */
        Base() : name{}, code{}, linker{}, base{}, vector_atom_deleted{}, pair_name{} {};

        /**
        * @brief Create Base from basic set of parameters
        * @param name name of the base
        * @param code three-letter code
        * @param file_path path to the backbone file
        * @param linker indices for atoms forming the vector connecting to the backbone
        * @param pair_name Name of the pairing base
        */

        Base(std::string name, std::string code, std::string file_path, std::array<std::size_t, 2> linker,
             std::string pair_name = "");

        /**
        * @brief Gives the atom of the base that connects directly to the backbone, Base::linker[0]
        * @return A pointer to the atom that connects to the backbone
        */
        OpenBabel::OBAtom* getLinker() {
            return base.GetAtom(static_cast<unsigned>(linker[0]));
        }

        /**
        * @brief Gives the (most likely hydrogen) atom of the base connected to the atom from getLinker() which defines how
        * the base connects, Base::linker[1]
        * @return A pointer to the atom forming the vector connecting to the backbone
        */
        OpenBabel::OBAtom* getVector() {
            if (!vector_atom_deleted)
                return base.GetAtom(static_cast<unsigned>(linker[1]));
            else
                return nullptr;
        }

        /**
        * @brief Returns a copy of the base molecule, Base::base
        * @return A copy of the base molecule
        */
        OpenBabel::OBMol getMolecule() {
            return base;
        }

        /**
        * @brief Gives the three-letter code of the base, Base::code
        * @return The code of the base
        */
        std::string getCode() {
            return code;
        }

        /**
        * @brief Gives the full name of the base, Base::name
        * @return The full name of the base
        */
        std::string getName() {
            return name;
        }

        /**
        * @brief Deletes the atom from getVector() safely. If the atom is already deleted, nothing happens.
        */
        void deleteVectorAtom() {
            if (!vector_atom_deleted) {
                size_t id = getLinker()->GetId();
                base.DeleteAtom(getVector());
                linker = {base.GetAtomById(id)->GetIdx(), 0};
                vector_atom_deleted = true;
            }
        }

        /**
        * @brief Get the name of the pair base, Base::pair_name
        * @return Pair name
        */
        std::string getBasePairName() {
            return pair_name;
        }

        std::string name,                               //!< @brief Full name of base (i.e. "Adenine" or just "A")
                    code,                               //!< @brief Three character code to define base ("Adenine": "ADE")
                    pair_name,                          //!< @brief Name of the pair base
                    file_path;                          //!< @brief Path to a file containing the base
        OpenBabel::OBMol base;                          //!< @brief The OBMol defining the base
        std::array<std::size_t, 2 > linker;             //!< @brief Holds indices for atoms forming a vector to connect to backbone {linker, hydrogen}

    private:
        /**
        * @brief Does some basic sanity checks (such as whether or not the indices of the atom are within the range
        * of the molecule).
        */
        void validate();

        bool vector_atom_deleted;                       //!< @brief Whether or not the getVector() atom was deleted
    };

    /**
    * @brief A class that contains a vector of all the defined bases and a funtion to
    * return a base and the complimentary base for all the bases defined.
    *
    * @sa Base
    */
    class Bases {
    public:
        /**
        * @brief Basic constructor for the Bases.
        *
        * The given bases are processed to create a vector of the defined bases and a vector of the complimentary bases.
        * This calls Base to make sure the given bases have Openbabel molecules
        *
        * @param input_bases The vector containing the information about the bases needed
        */
        Bases(std::vector<Base> input_bases);

        /**
        * @brief Empty constructor.
        */
        Bases() {};

        /**
        * @brief Returns the Base instance given the name of the base.
        *
        * @param name name of the base
        *
        * @returns the base
        */
        PNAB::Base getBaseFromName(std::string name) {
            for (auto v : bases) {
                if (v.getName().find(name) != std::string::npos)
                    return v;
            }
            std::cerr << "Base \"" << name << "\" does not exists in list of bases. Please check input file."
                      << std::endl;
            throw 1;
        }

        /**
        * @brief Returns the vector of the instances of Base given the names of the bases in the strand.
        *
        * @param strand vector of base names in the strand
        *
        * @returns vector of bases in the strand
        *
        * @sa Chain::Chain
        */
        std::vector<Base> getBasesFromStrand(std::vector<std::string> strand);

        /**
        * @brief Returns the complimentary vector of the instances of Base given the names of the bases in the strand.
        *
        * @param strand vector of base names in the strand
        *
        * @returns vector of bases in the complimentary strand
        *
        * @sa Chain::Chain
        */
        std::vector<Base> getComplimentBasesFromStrand(std::vector<std::string> strand);

    private:
        std::vector<Base> bases;                          //!< @brief The vector of bases
        bool all_bases_pair;                              //!< @brief Whether all the bases in the strand have complimentary bases
        std::map<std::string, PNAB::Base> name_base_map;  //!< @brief A map of the names of the bases and the complimentary bases
    };

    /**
    * @brief Class to hold bases with backbones attached (nucleotides), along with associated necessary information
    *
    * @sa Base
    * @sa Backbone
    * @sa Chain::setupChain
    * @sa ConformationSearch
    */
    class BaseUnit {
    public:
        /**
        * Constructor for the base unit
        *
        * @param b Base instance
        * @param backbone Backbone instance
        * @param glycosidic_bond_distance The glycosidic bond distance. If zero (default), set it by using the van der Waals radii
        */
        BaseUnit(Base b, Backbone backbone, double glycosidic_bond_distance = 0.0);

        /**
        * @brief Empty constructor.
        */
        BaseUnit() {};

        /**
        * @brief Returns the nucleotide molecule, BaseUnit::unit
        *
        * @return The nucleotide molecule
        */
        const OpenBabel::OBMol getMol() {
            return unit;
        }

        /**
        * @brief Returns the indices for the begining and end of the nucleobase atom indices, BaseUnit::base_index_range
        *
        * @return The indices for the limits of the nucleobases
        */
        const std::array< std::size_t, 2 > getBaseIndexRange() {
            return base_index_range;
        };

        /**
        * @brief Returns the indices for the begining and end of the backbone atom indices, BaseUnit::backbone_index_range
        *
        * @return The indices for the limits of the backbone
        */
        const std::array< std::size_t, 2 > getBackboneIndexRange() {
            return backbone_index_range;
        };

        /**
        * @brief Returns the indices for atoms where the backbone connects, BaseUnit::backbone_interconnects
        *
        * @return The indices for atoms where the backbone connects
        */
        const std::array< std::size_t, 2 > getBackboneLinkers() {
            return backbone_interconnects;
        };

        /**
        * @brief Returns the index of the atom where the nucleobase connects to the backbone, BaseUnit::base_connect_index
        *
        * @return The nucleobase index connecting to the backbone
        */
        std::size_t getBaseConnectIndex() {
            return base_connect_index;
        }

        /**
        * @brief Returns a vector of the pair of indices for fixed rotatable dihedrals in the backbone, BaseUnit::fixed_bonds
        *
        * @return The indices of the fixed rotatable dihedrals
        */
        std::vector<std::vector<unsigned>> getFixedBonds() {
            return fixed_bonds;
        }

    private:
        OpenBabel::OBMol unit;                                                       //!< @brief Holds molecule containing base with backbone attached
        std::array< std::size_t, 2 > base_index_range,                               //!< @brief Range of indices of the unit that are a part of the base, [start, stop]
                                     backbone_index_range;                           //!< @brief Range of indices of the unit that are a part of the backbone, [start, stop]
        std::size_t base_connect_index;                                              //!< @brief Atom index where backbone connects to base (the base atom)
        std::array< std::size_t, 2 > backbone_interconnects;                         //!< @brief Atom indices defining where backbone connects
        std::vector<std::vector<unsigned>> fixed_bonds;                              //!< @brief Indices of fixed bonds in dihedral search
    };

    /**
     * @brief Class to contain important information for an individual conformer
     *
     * It includes detailed information about the energy components important
     * for distinguishing between different conformers. It also includes the value
     * of the RMSD relative to the best candidate. If the conformer satisfies
     * the distance and energy thresholds, then it is saved. It also contains
     * the openbabel OBMol object for the accepted candidates
     *
     * @sa Chain::generateConformerData
     * @sa ConformationSearch::reportData
     * @sa RuntimeParameters::energy_filter
     * @sa RuntimeParameters::max_distance
     */
    struct ConformerData {
        OpenBabel::OBMol molecule;                  //!< @brief The openbabel OBMol object for the conformer
        double *monomer_coord,                      //!< @brief Pointer to array containing coordinates of a single monomer
                distance,                           //!< @brief distance between interconnects in Backbone for adjacent BaseUnit
                bondE,                              //!< @brief Energy of newly formed bonds in the backbone divided by the length of the strand -1
                angleE,                             //!< @brief Energy of newly formed angles in the backbone divided by the length of the strand -1
                torsionE,                           //!< @brief Energy of all rotatable torsions divided by the length of the strand
                VDWE,                               //!< @brief Total van Der Wals Energy divided by the length of the strand
                total_energy,                       //!< @brief Total energy of the conformation divided by divided by the length of the strand
                rmsd;                               //!< @brief Root-mean square distance relative to lowest energy conformer
        std::size_t index;                          //!< @brief The index of the conformer
        bool accepted;                              //!< @brief Is the energy of the conformer less than the thresholds
        std::vector<double> dihedral_angles;        //!< @brief The values of the rotatable dihedral angles in the conformer in degrees

        /**
         * @brief Used for simple sorting based on total energy of the conformer
         * @param cd The ConformerData element to compare current element to
         * @return True if the other ConformerData has greater total energy, false otherwise
         */
        bool operator<(const ConformerData &cd) const {
            return (total_energy < cd.total_energy);
        }
    };

}

#endif //PNAB_CONTAINERS_H
