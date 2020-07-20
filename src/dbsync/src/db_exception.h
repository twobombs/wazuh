/*
 * Wazuh DBSYNC
 * Copyright (C) 2015-2020, Wazuh Inc.
 * June 11, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#pragma once
#include <stdexcept>
#include <string>
#include <map>

constexpr auto FACTORY_INSTANTATION     { std::make_pair(1, "Unspecified type during factory instantiation") }; 
constexpr auto INVALID_HANDLE           { std::make_pair(2, "Invalid handle value.") }; 
constexpr auto INVALID_TRANSACTION      { std::make_pair(3, "Invalid transaction value.") }; 
constexpr auto SQLITE_CONNECTION_ERROR  { std::make_pair(4, "No connection available for executions.") }; 
constexpr auto EMPTY_DATABASE_PATH      { std::make_pair(5, "Empty database store path.") }; 
constexpr auto EMPTY_TABLE_METADATA     { std::make_pair(6, "Empty table metadata.") }; 

namespace DbSync
{
    /**
     *   This class should be used by concrete types to report errors.
    */
    class dbsync_error : public std::exception
    {
      public:
        __attribute__((__returns_nonnull__))
        const char* what() const noexcept override
        {
            return m_error.what();
        }

        int32_t id() const noexcept
        {
            return m_id;
        }

        dbsync_error(const int32_t id,
                     const std::string& whatArg)
        : m_id{ id }
        , m_error{ whatArg }
        {}

        explicit dbsync_error(const std::pair<int32_t, std::string>& exceptionInfo)
        : m_id{ exceptionInfo.first }
        , m_error{ exceptionInfo.second }
        {}

      private:
        /// an exception object as storage for error messages
        const int32_t m_id;
        std::runtime_error m_error;
    };
}