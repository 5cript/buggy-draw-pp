#pragma once

#ifndef Q_MOC_RUN // A Qt workaround, for those of you who use Qt
#   include "SimpleJSON/parse/jsd.hpp"
#   include "SimpleJSON/parse/jsd_convenience.hpp"
#   include "SimpleJSON/stringify/jss.hpp"
#   include "SimpleJSON/stringify/jss_fusion_adapted_struct.hpp"
#endif

template <typename T, typename InputT>
T parse(InputT& json, std::string const& name)
{
    T result;
    auto tree = JSON::parse_json(json);
    JSON::parse(result, name, tree);
    return result;
}

template <typename T>
std::ostream& stringify(std::ostream& stream, T const& toStringify, std::string const& name)
{
    JSON::try_stringify(stream, name, toStringify);
    return stream;
}
