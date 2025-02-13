#ifndef INPUT_OUTPUT_PARTITION_H
#define INPUT_OUTPUT_PARTITION_H

#include <stdexcept>
#include <vector>

namespace Syft {

/**
 * \brief A partition of variables into input and output variables.
 */
class InputOutputPartition {
private:
  
  static std::runtime_error bad_file_format_exception(std::size_t line_number);
  static std::runtime_error error_open_file_exception(std::string filename);
  
public:
  
  std::vector<std::string> input_variables;
  std::vector<std::string> output_variables;

  /**
   * \brief Creates a partition with no variables.
   */
  InputOutputPartition();

   /**
    * \brief Constructs a partition from a file.
    *                                                  
    * The file should look like
    *   .inputs: X1 X2 X3 X4
    *   .outputs: Y1 Y2 Y3
    *
    * \param filename The name of the partition file.    
    * \return A partition with the input and output variables listed in the file
    */
  static InputOutputPartition read_from_file(const std::string& filename);
};

}

#endif // INPUT_OUTPUT_PARTITION_H
