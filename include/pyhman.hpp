#ifndef __PY_CLV_HD_HPP__
#define __PY_CLV_HD_HPP__

#include "hman.hpp"

#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(pyhman)
{
    uint16_t (ClvHd::Master::*v1)(int) = &ClvHd::Master::getVersion;
    std::string (ClvHd::Master::*v2)() = &ClvHd::Master::getVersion;
    class_<ClvHd::Master>("Master", init<int>())
        .def("printBit", &ClvHd::Master::printBit)
        .def("readReg8", &ClvHd::Master::readReg8, args("id", "reg"),
             "Read 8-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg16", &ClvHd::Master::readReg16, args("id", "reg"),
             "Read 16-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg32", &ClvHd::Master::readReg32, args("id", "reg"),
             "Read 32-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg64", &ClvHd::Master::readReg64, args("id", "reg"),
             "Read 64-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readRegFloat", &ClvHd::Master::readRegFloat, args("id", "reg"),
             "Read 32-bit float register\n\n:param id: ID of the EMG "
             "module\n:param reg: Register address\n:return: Register value")
        .def("readRegDouble", &ClvHd::Master::readRegDouble, args("id", "reg"),
             "Read 64-bit double register\n\n:param id: ID of the EMG "
             "module\n:param reg: Register address\n:return: Register value")
        .def("writeReg", &ClvHd::Master::writeReg, args("id", "reg", "val"),
             "Write 8-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:param val: Value to write")
        .def("getNbModules", &ClvHd::Master::getNbModules,
             "Get the number of modules connected to the master board")
        .def("test_connection", &ClvHd::Master::test_connection,
             "Test the connection to the master board")
        .def("getVersion", v1, args("i"),
             "Get the version of the master board\n\n:param i: Index of the "
             "version value. For i = 1, the major version is returned. For i = "
             "2, the minor version is returned. For i = 0, a short composed of "
             "the major and minor version is returned.\n:return: The version "
             "of the master board. If 0 is returned, an error occured.")
        .def("getVersion", v2,
             "Get the version of the master board\n\n:return: The version of "
             "the master board as a std::string. If an empty string is "
             "returned, an error occured.")
        .def("blink", &ClvHd::Master::blink, args("id", "duration", "nb"),
             "Blink the LED of the module with the given id\n\n:param id: ID "
             "of the EMG module\n:param duration: Duration of the blink in "
             "cs\n:param nb: Number of blink")
        .def("setup", &ClvHd::Master::setup, "Setup the master board")
        .def("setupEMG", &ClvHd::Master::setupEMG,
             args("id", "route_table", "chx_enable", "chx_high_res",
                  "chx_high_freq", "R1", "R2", "R3"),
             "Setup the EMG module with the given id\n\n:param id: Id of the "
             "EMG module to setup\n:param route_table: Route table of the EMG "
             "module\n:param chx_enable: Enable/disable of the EMG "
             "module\n:param chx_high_res: High resolution of the EMG "
             "module\n:param chx_high_freq: High frequency of the EMG "
             "module\n:param R1: R1 of the EMG module\n:param R2: R2 of the "
             "EMG module\n:param R3: R3 of the EMG module")
        .def("data_ready", &ClvHd::Master::data_ready,
             args("id", "channel", "precise"),
             "Check if the data is ready to be read from from the given "
             "channel of the module with the given id.\n\n:param id: Id of the "
             "module to check.\n:param channel: Channel to check.\n:param "
             "precise: If true, the function will return true if the precise "
             "signal (3bytes) of the given channel is ready to be read. If "
             "false, the function will return true if the fast signal (2byte) "
             "of the given channel is ready to be read.\n:return: true if the "
             "data is ready to be read, false otherwise.")
        .def("read_fast_EMG", &ClvHd::Master::read_fast_EMG,
             args("id", "channel"),
             "read_fast_data Read (actual request to the master board) the "
             "fast data(2byte) from the given channel of the module with the "
             "given id.\n\n:param id: Id of the module to read.\n:param "
             "channel: Channel to read.\n:return: The fast data(2byte) from "
             "the given channel of the module with the given id.")
        .def("read_precise_EMG", &ClvHd::Master::read_precise_EMG,
             args("id", "channel"),
             "read_precise_data Read (actual request to the master board) the "
             "precise data(3byte) from the given channel of the module with "
             "the given id.\n\n:param id: Id of the module to read.\n:param "
             "channel: Channel to read.\n:return: The precise data(3byte) "
             "from the given channel of the module with the given id.")
        .def("fast_EMG", &ClvHd::Master::fast_EMG, args("id", "channel"),
             "fast_EMG Read the previously requested fast data(2byte) from the "
             "given channel of the module with the given id.\n\n:param id: Id "
             "of the module to read.\n:param channel: Channel to "
             "read.\n:return: The fast data(2byte) from the given channel of "
             "the module with the given id.")
        .def(
            "precise_EMG", &ClvHd::Master::precise_EMG, args("id", "channel"),
            "precise_EMG Read the previously requested precise data(3byte) "
            "from the given channel of the module with the given id.\n\n:param "
            "id: Id of the module to read.\n:param channel: Channel to "
            "read.\n:return: The precise data(3byte) from the given channel "
            "of the module with the given id.")
        .def("start_acquisition", &ClvHd::Master::start_acquisition,
             "Set the "
             "modules in acquisition mode")
        .def("stop_acquisition", &ClvHd::Master::stop_acquisition,
             "Set the "
             "modules in standby mode")
        .def("read_all_signal", &ClvHd::Master::read_all_signal,
             "Read all data registers (State, 3fast, 3precise) from each "
             "available module.")
        .def(
            "get_error", &ClvHd::Master::get_error, args("id", "verbose"),
            "Get the active error from the module with the given id.\n\n:param "
            "id: Id of the module to read.\n:param verbose: If true, If true, "
            "the function will return a more detailed error message.\n:return: "
            "The active error from the module with the given id.")
        .def("__str__", &ClvHd::Master::operator std::string)
        .def("__repr__", &ClvHd::Master::repr);

    // double (ClvHd::EMG::*conv1)(uint16_t) = &ClvHd::EMG::conv;
    // double (ClvHd::EMG::*conv2)(int, int32_t) = &ClvHd::EMG::conv;
    // class_<ClvHd::EMG>("EMG", init<ClvHd::Master*, int>())
    // .def("setup", &ClvHd::EMG::setup)
    // .def("route_channel", &ClvHd::EMG::route_channel)
    // .def_readonly("m_mode", &ClvHd::EMG::get_mode)
    // .def("config_clock", &ClvHd::EMG::config_clock)
    // .def("enable_channels", &ClvHd::EMG::enable_channels)
    // .def("enable_SDM", &ClvHd::EMG::enable_SDM)
    // .def("enable_INA", &ClvHd::EMG::enable_INA)
    // .def("config_frequence", &ClvHd::EMG::config_frequence)
    // .def("config_resolution", &ClvHd::EMG::config_resolution)
    // .def("config_R1", &ClvHd::EMG::config_R1)
    // .def("config_R2", &ClvHd::EMG::config_R2)
    // .def("config_R3", &ClvHd::EMG::config_R3)
    // .def("read_precise_value", &ClvHd::EMG::read_precise_value)
    // .def("read_fast_value", &ClvHd::EMG::read_fast_value)
    // .def("precise_value", &ClvHd::EMG::precise_value)
    // .def("fast_value", &ClvHd::EMG::fast_value)
    // .def("conv", conv1)
    // .def("conv", conv2)
    // .def("get_error", &ClvHd::EMG::get_error)
    // .def("error_range_str", &ClvHd::EMG::error_range_str)
    // .def("error_status_str", &ClvHd::EMG::error_status_str);
}

#endif /* __PY_CLV_HD_HPP__ */
