// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBHelper.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Path.h"

// C/C++ include files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include "TGeoManager.h"
#include "TRint.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

static void check_result(long result)   {
  if ( 0 == result )
    except("DDDBExecutor","+++ Odd things going on.....");
  //result = *(long*)result;
  if ( result != 1 )
    except("DDDBExecutor","+++ Odd things going on.....Processing result=%ld",result);
}

static void usage_load_xml_dddb()   {
  cout <<
    "DDDBExecutor opt [opt]\n"
    "   Note: No '-' signs infront of identifiers!                           \n"
    "                                                                        \n"
    "  -loader <plugin>      Plugin instance for XML entity resolution.      \n"
    "  -param  <file-name>   Preprocessing xml file                          \n"
    "  -input  <file-name>   Directory containing DDDB                       \n"
    "  -config <plugin>      Execute config plugin initializing the helper.  \n"
    "  -match  <string>      Match string for entity resolver e.g.'conddb:'  \n"
    "  -xml    <file-name>   Parse additional XML files using LCDD.          \n"
    "  -setup  <plugin>      Add setup plugin after dddb parsing.            \n"
    "  -exec   <plugin>      Add execution plugin after setup.               \n"
    "  -attr   <file-name>   Optional XML parsing for visualization attrs.   \n"
    "  -dump                 Call DDDB_DetectorConditionDump plugin.         \n"
    "  -visualize            Call display after processing                   \n"
    ""
    "" << endl;

  ::exit(EINVAL);
}

static long run_plugin(Geometry::LCDD& lcdd, const std::string& plugin, std::vector<char*>& args)  {
  args.push_back(0);
  return lcdd.apply(plugin.c_str(), args.size()-1, &args[0]);
}

static long load_xml_dddb(Geometry::LCDD& lcdd, int argc, char** argv) {
  if ( argc > 0 )   {
    string sys_id, params, match="conddb:", attr="", loader_name="DDDB_FileReader";
    std::vector<string> setup, xmlFiles, executors, config;
    std::map<std::string, std::vector<char*> > e_args, s_args, c_args;
    long result = 0, visualize = 0, dump = 0;
    char last = 0, c;
    for(int i=0; i<argc;++i) {
      c = 0;
      if ( argv[i][0] == '-' )  {
        c = ::toupper(argv[i][1]);
        switch(c)  {
        case 'A':
          attr = argv[++i];
          last = 0;
          break;
        case 'C':
          config.push_back(argv[++i]);
          c_args[config.back()] = std::vector<char*>();
          last = c;
          break;
        case 'D':
          dump = 1;
          last = 0;
          break;
        case 'E':
          executors.push_back(argv[++i]);
          e_args[executors.back()] = std::vector<char*>();
          last = c;
          break;
        case 'I':
          sys_id = argv[++i];
          last = 0;
          break;
        case 'L':
          loader_name = argv[++i];
          last = 0;
          break;
        case 'M':
          match = argv[++i];
          last = 0;
          break;
        case 'P':
          params = argv[++i];
          last = 0;
          break;
        case 'S':
          setup.push_back(argv[++i]);
          s_args[setup.back()] = std::vector<char*>();
          last = c;
          break;
        case 'V':
          visualize = 1;
          last = 0;
          break;
        case 'X':
          xmlFiles.push_back(argv[++i]);
          last = 0;
          break;
        case 'H':
          usage_load_xml_dddb();
          break;
        case '?':
          usage_load_xml_dddb();
          break;
        default:
          usage_load_xml_dddb();
        }
      }
      if ( !c && last == 'C' )
        c_args[config.back()].push_back(argv[i]);
      else if ( !c && last == 'E' )
        e_args[executors.back()].push_back(argv[i]);
      else if ( !c && last == 'S' )  {
        s_args[setup.back()].push_back(argv[i]);
      }
    }

    Path path = sys_id;
    sys_id = path.normalize().c_str();

    /// Install helper
    {
      lcdd.apply("DDDB_InstallHelper", 0, 0);
    }

    DDDBHelper* helper = lcdd.extension<DDDBHelper>();
    if ( !loader_name.empty() )  {
      DDDBReader* resolver = (DDDBReader*)DD4hep::PluginService::Create<void*>(loader_name,(const char*)0);
      resolver->setMatch(match);
      resolver->setDirectory(path.parent_path().c_str());
      helper->setXmlReader(resolver);
    }

    /// Execute config plugins without arguments
    for(size_t i=0; i<config.size(); ++i)
      run_plugin(lcdd, config[i], c_args[config[i]]);
    
    /// Pre-Process Parameters
    if ( !params.empty() )    {
      const void* args[] = {0, params.c_str(), 0};
      printout(INFO,"DDDBExecutor","++ Processing parameters: %s",params.c_str());
      result = lcdd.apply("DDDB_Loader", 2, (char**)args);
      check_result(result);
    }

    /// Process visualization attributes. Must be present before converting the geometry!
    if ( !attr.empty() )  {
      const void* args[] = {attr.c_str(), 0};
      printout(INFO,"DDDBExecutor","+++ Processing visualization attributes: %s", attr.c_str());
      result = lcdd.apply("DD4hepXMLLoader", 1, (char**)args);
      check_result(result);
    }

    /// Process XML
    if ( !sys_id.empty() )   {
      long long int init_time = makeTime(2016,4,1,12);
      const void* args[] = {0, sys_id.c_str(), "/", &init_time, 0};
      printout(INFO,"DDDBExecutor","+++ Processing DDDB: %s", sys_id.c_str());
      result = lcdd.apply("DDDB_Loader", 4, (char**)args);
      check_result(result);
      printout(INFO,"DDDBExecutor","                         .... done");
    }

    /// Convert local database to TGeo
    {
      result = lcdd.apply("DDDB_2DD4hep", 0, 0);
      check_result(result);
    }

    /// Load XML files as wanted (may also be plugins)
    if ( !xmlFiles.empty() )  {
      for(size_t i=0; i<xmlFiles.size(); ++i)  {
        const void* args[] = {xmlFiles[i].c_str(), 0};
        lcdd.apply("DD4hepXMLLoader", 1, (char**)args);
      }
    }

    /// Execute further setup plugins without arguments
    for(size_t i=0; i<setup.size(); ++i)
      run_plugin(lcdd, setup[i], s_args[setup[i]]);

    /// Call executors
    for(size_t i=0; i<executors.size(); ++i)
      run_plugin(lcdd, executors[i], e_args[executors[i]]);

    if ( dump )    {
      printout(INFO,"DDDBExecutor","------------------> Conditions dump:");
      lcdd.apply("DDDB_DetectorConditionDump", 0, 0);
    }
    if ( visualize )   { /// Fire off display
      pair<int, char**> a(0,0);
      TRint app("DDDB", &a.first, a.second);
      TGeoManager& mgr = lcdd.manager();
      mgr.SetVisLevel(999);
      mgr.SetVisOption(1);
      lcdd.worldVolume()->Draw("ogl");
      app.Run();
    }
  }
  return 1;
}
DECLARE_APPLY(DDDB_Executor,load_xml_dddb)
