// @file:     problem.cc
// @author:   Samuel
// @created:  2017.08.23
// @editted:  2017.08.23 - Samuel
// @license:  GNU LGPL v3
//
// @desc:     Definition of the problem - dbdot loc, material properties, etc.

#include "problem.h"

using namespace phys;

// Constructors
Problem::Problem(const std::string &fname) 
{
  initProblem();
  readProblem(fname);
}

void Problem::initProblem() {
  db_tree = std::make_unique<Problem::Aggregate>();
}


// File handling

// parse problem XML, return true if successful
bool Problem::readProblem(const std::string &fname)
{
  std::cout << "Reading problem file: " << fname << std::endl;

  std::ifstream in_file(fname);
  rapidxml::xml_document<> xmldoc;
  rapidxml::xml_node<> *root_node;

  if(!in_file){
    std::cout << "Failed to open file " << fname << std::endl;
    return false;
  }


  // read file to buffer and close
  std::stringstream buffer;
  buffer << in_file.rdbuf();
  in_file.close();
  std::string in_content(buffer.str());

  //std::cout << "Buffer content: " << in_content << std::endl;

  // parse XML
  std::cout << "Parse XML" << std::endl;
  xmldoc.parse<0>(&in_content[0]);
  root_node = xmldoc.first_node(); // get root node of input file
  std::cout << "Root node name: " << root_node->name() << std::endl;

  // material parameters
  std::cout << "Read material properties" << std::endl;
  readMaterialProp(root_node->first_node("material_prop"));

  // simulation parameters
  std::cout << "Read simulation parameters" << std::endl;
  readSimulationParam(root_node->first_node("sim_param"));

  // item tree
  std::cout << "Read DB tree" << std::endl;
  readItemTree(root_node->first_node("db_tree"), db_tree);

  return true;
}


bool Problem::readMaterialProp(rapidxml::xml_node<> *node)
{
  for(rapidxml::xml_node<> *material_node = node->first_node(); material_node; material_node = material_node->next_sibling()){
    std::cout << "Material property: " << material_node->name() << std::endl;
    // TODO material vector, containing structs of material properties
  }
  return true;
}


bool Problem::readSimulationParam(rapidxml::xml_node<> *node)
{
  for(rapidxml::xml_node<> *sim_node = node->first_node(); sim_node; sim_node = sim_node->next_sibling()){
    // TODO just add everything into sim_param dictionary
  }
  return true;
}


bool Problem::readItemTree(rapidxml::xml_node<> *node, const std::unique_ptr<Aggregate>& agg_parent)
{
  std::cout << "Entered readItemTree" << std::endl;
  for(rapidxml::xml_node<> *item_node = node->first_node(); item_node; item_node = item_node->next_sibling()){
    std::string item_name = item_node->name();
    std::cout << "Item name: " << item_name << std::endl;

    if(!item_name.compare("aggregate")){
      std::cout << "Entered Aggregate" << std::endl;
      // add aggregate child to tree
      std::cout << "db_tree pointer " << db_tree.get() << std::endl;
      agg_parent->aggs.push_back(std::make_unique<Aggregate>());
      readItemTree(item_node, agg_parent->aggs.back());
      std::cout << "Ended Aggregate" << std::endl;
    }
    else if(!item_name.compare("dbdot")) {
      std::cout << "Entered DBDot" << std::endl;
      // add DBDot to tree
      readDBDot(item_node, agg_parent);
      std::cout << "Ended DBDot" << std::endl;
    }
    else
      std::cout << "Encountered unknown item_node: " << item_node->name() << std::endl;
  }
  return true;
}


bool Problem::readDBDot(rapidxml::xml_node<> *node, const std::unique_ptr<Aggregate>& agg_parent)
{
  std::cout << "Entered readDBDot" << std::endl;
  float x,y;

  // read x and y from XML stream
  x = std::stof(node->first_attribute("x")->value());
  y = std::stof(node->first_attribute("y")->value());

  std::cout << "DBDot read with x=" << x << ", y=" << y << std::endl;
  std::cout << "agg_parent address: " << agg_parent.get() << std::endl;

  agg_parent->dbdots.push_back(std::make_unique<DBDot>(x,y));

  std::cout << "DBDot created" << std::endl;

  return true;
}

// write result to XML, return true if successful
bool Problem::writeResult()
{
  return true;
}
