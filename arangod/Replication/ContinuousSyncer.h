////////////////////////////////////////////////////////////////////////////////
/// @brief replication continuous data synchronizer
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_REPLICATION_CONTINUOUS_SYNCER_H
#define ARANGODB_REPLICATION_CONTINUOUS_SYNCER_H 1

#include "Basics/Common.h"
#include "Replication/Syncer.h"
#include "Utils/ReplicationTransaction.h"
#include "VocBase/replication-applier.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

struct TRI_json_t;
struct TRI_server_t;
struct TRI_vocbase_t;

namespace triagens {

  namespace httpclient {
    class SimpleHttpResult;
  }

 namespace arango {
    class ReplicationTransaction;

    enum RestrictType : uint32_t {
      RESTRICT_NONE,
      RESTRICT_INCLUDE,
      RESTRICT_EXCLUDE
    };

// -----------------------------------------------------------------------------
// --SECTION--                                                  ContinuousSyncer
// -----------------------------------------------------------------------------

    class ContinuousSyncer : public Syncer {

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ContinuousSyncer (TRI_server_t*,
                          TRI_vocbase_t*,
                          struct TRI_replication_applier_configuration_s const*,
                          TRI_voc_tick_t,
                          bool); 

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ContinuousSyncer ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief run method, performs continuous synchronization
////////////////////////////////////////////////////////////////////////////////

        int run ();

////////////////////////////////////////////////////////////////////////////////
/// @brief return the syncer's replication applier
////////////////////////////////////////////////////////////////////////////////
        
        TRI_replication_applier_t* applier () const {
          return _applier;
        }

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief set the applier progress
////////////////////////////////////////////////////////////////////////////////

        void setProgress (char const*);
        
        void setProgress (std::string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief save the current applier state
////////////////////////////////////////////////////////////////////////////////

        int saveApplierState ();

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not a collection should be excluded
////////////////////////////////////////////////////////////////////////////////

        bool skipMarker (TRI_voc_tick_t,
                         struct TRI_json_t const*) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not a collection should be excluded
////////////////////////////////////////////////////////////////////////////////

        bool excludeCollection (std::string const&) const;
       
////////////////////////////////////////////////////////////////////////////////
/// @brief get local replication applier state
////////////////////////////////////////////////////////////////////////////////

        int getLocalState (std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief starts a transaction, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int startTransaction (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief aborts a transaction, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int abortTransaction (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief commits a transaction, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int commitTransaction (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief process a document operation, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int processDocument (TRI_replication_operation_e,
                             struct TRI_json_t const*,
                             std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief renames a collection, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int renameCollection (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief changes the properties of a collection, based on the JSON provided
////////////////////////////////////////////////////////////////////////////////

        int changeCollection (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief apply a single marker from the continuous log
////////////////////////////////////////////////////////////////////////////////

        int applyLogMarker (struct TRI_json_t const*,
                            TRI_voc_tick_t,
                            std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief apply the data from the continuous log
////////////////////////////////////////////////////////////////////////////////

        int applyLog (httpclient::SimpleHttpResult*,
                      TRI_voc_tick_t,
                      std::string&,
                      uint64_t&,
                      uint64_t&);

////////////////////////////////////////////////////////////////////////////////
/// @brief perform a continuous sync with the master
////////////////////////////////////////////////////////////////////////////////

        int runContinuousSync (std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief fetch the initial master state
////////////////////////////////////////////////////////////////////////////////

        int fetchMasterState (std::string&,
                              TRI_voc_tick_t,
                              TRI_voc_tick_t,
                              TRI_voc_tick_t&);

////////////////////////////////////////////////////////////////////////////////
/// @brief run the continuous synchronization
////////////////////////////////////////////////////////////////////////////////

        int followMasterLog (std::string&,
                             TRI_voc_tick_t&,
                             TRI_voc_tick_t,
                             uint64_t&,
                             bool&,
                             bool&);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief server
////////////////////////////////////////////////////////////////////////////////

        TRI_server_t* _server;

////////////////////////////////////////////////////////////////////////////////
/// @brief pointer to the applier state
////////////////////////////////////////////////////////////////////////////////

        TRI_replication_applier_t* _applier;

////////////////////////////////////////////////////////////////////////////////
/// @brief stringified chunk size
////////////////////////////////////////////////////////////////////////////////

        std::string _chunkSize;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection restriction type
////////////////////////////////////////////////////////////////////////////////

        RestrictType _restrictType;

////////////////////////////////////////////////////////////////////////////////
/// @brief initial tick for continuous synchronization
////////////////////////////////////////////////////////////////////////////////

        TRI_voc_tick_t _initialTick;

////////////////////////////////////////////////////////////////////////////////
/// @brief use the initial tick
////////////////////////////////////////////////////////////////////////////////

        bool _useTick;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not to replicate system collections
////////////////////////////////////////////////////////////////////////////////

        bool _includeSystem;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the specified from tick must be present when fetching
/// data from a master
////////////////////////////////////////////////////////////////////////////////

        bool _requireFromPresent;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the applier should be verbose
////////////////////////////////////////////////////////////////////////////////

        bool _verbose;

////////////////////////////////////////////////////////////////////////////////
/// @brief which transactions were open and need to be treated specially
////////////////////////////////////////////////////////////////////////////////

        std::unordered_map<TRI_voc_tid_t, ReplicationTransaction*> _ongoingTransactions;

    };

  }
}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
