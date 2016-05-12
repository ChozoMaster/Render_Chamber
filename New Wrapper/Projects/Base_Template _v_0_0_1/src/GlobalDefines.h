#include <vector>
#include <Eigen/Core>
#include <map>
typedef std::vector<Eigen::Vector3f>				v3f_list;
typedef std::vector<std::array<unsigned int, 3> >	v3i_list;
typedef std::map<std::string, v3f_list>				v3f_list_catalog;
typedef std::map<std::string, v3f_list>::iterator	v3f_list_catalog_itr;
typedef std::map<std::string, v3i_list>::iterator	v3i_list_catalog_itr;
