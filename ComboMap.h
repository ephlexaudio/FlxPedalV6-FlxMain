/*
 * ComboMap.h
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */

#ifndef COMBOMAP_H_
#define COMBOMAP_H_

#include "ComboDataInt.h"
#include "FileSystemInt.h"
namespace std
{

	class ComboMap : public ComboDataInt
	{
	private:
		//ProcessUtility procUtil;
		map<string, ComboDataInt> comboDataMap;
		vector<string> comboNameVector;
		FileSystemInt fsInt;
	protected:


	public:
		ComboMap ();
		~ComboMap ();

		int loadComboMapAndList(void);
		int loadProcessUtilities(ProcessUtility procUtil);
		std::vector<string> getComboNameList(void);
		int addNewComboObjectToMapAndList(string comboName);
		int addNewComboObjectToMap(string comboName);
		int loadCombosFromFileSystemToComboMap();
		int deleteComboObjectFromMapAndList(string comboName);
		ComboDataInt getComboObject(string comboName);
		bool isInComboMap(string comboName);
		void eraseFromMap(string comboName);
		int saveCombo(string comboName, ComboStruct comboData);
	};

} /* namespace std */

#endif /* COMBOMAP_H_ */
