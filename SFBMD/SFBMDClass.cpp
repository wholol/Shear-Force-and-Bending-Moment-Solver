#include "pch.h"
#include "sfbmd.h"
#include <fstream>

ofstream parsedata("shear_force and bending moments.csv");


void beamProperties::initbeam() {	//insert beamlength
	cout << "Welcome to the shear force and bending moment solver. Enter beam length to get started." << endl;
	cin >> beamlength;
}

void beamProperties::PointLoadSetup() {		//insert number of point loads and values.
	cout << "Enter number of point loads that you have." << endl;
	cin >> PointLoadNumber;
	cout << "   " << endl;
	if (PointLoadNumber > 0) {
		cout << "Enter Point Load magnitude, followed by its location." << endl;
		for (int i = 0; i < PointLoadNumber; ++i) {
			cout << "   " << endl;
			cout << "enter point load magnitude." << endl;
			cin >> PointLoadMagnitude;
			PointLoadMagnitudeVect.push_back(PointLoadMagnitude);
			cout << "   " << endl;
			cout << "Enter Point Load location" << endl;
			cin >> PointLoadLocation;
			PointLoadLocationVect.push_back(PointLoadLocation);
		}
	}
}

void beamProperties::UDLSetup() {		//insert number of UDL loads
	cout << "Enter number of  UNIFORM distributed loads." << endl;
	cin >> UDLnumber;
	for (int i = 0; i < UDLnumber; i++) {
		cout << "Enter distributed loading w value, followed by its beginning location, lastly the end location." << endl;
		cout << " The beginning of the load is at the LEFT side of the beam, and the end at the left." << endl;
		cin >> UDLmagnitude;
		UDLmagnitudeVect.push_back(UDLmagnitude);
		cout << " " << endl;
		cout << "enter beginning of location (from left)." << endl;
		cin >> UDLbegin;
		UDLlocation_beginVect.push_back(UDLbegin);
		cout << " " << endl;
		cout << " enter end of location (from right)" << endl;
		cin >> UDLend;
		UDLlocation_endVect.push_back(UDLend);

	}
}

void beamProperties::TDLSetup() {
	cout << "Enter number of  TRIANGULAR distributed loads." << endl;
	cin >> TDLnumber;
	for (int i = 0; i < TDLnumber; i++) {
		cout << "Enter distributed loading w value, followed by its beginning location" << endl;
		cout << "the end location and where the top edge of the triangle is." << endl;
		cout << " The beginning of the load is at the LEFT side of the beam, and the end at the right." << endl;
		cout << " " << endl;
		cout << "enter w value." << endl;
		cin >> TDLmagnitude;
		TDLmagnitudeVect.push_back(TDLmagnitude);
		cout << " " << endl;
		cout << "enter beginning of location (from left)." << endl;
		cin >> TDLbegin;
		TDLlocation_beginVect.push_back(TDLbegin);
		cout << "enter end of location (from left)" << endl;
		cin >> TDLend;
		TDLlocation_endVect.push_back(TDLend);
		cout << "is the top edge of the triangle at the beginning or end location. If beginning, enter b, else enter e." << endl;
		cin >> TDLgradient;
		TDLgradientVect.push_back(TDLgradient);
	}
}

void beamProperties::SupportSetup() {
	SupportLocationArray[0] = 0;
	SupportLocationArray[1] = beamlength;
}

int beamProperties::getPointLoadNumber() const{
	return PointLoadNumber;
}

int beamProperties::getUDLNumber() const {
	return UDLnumber;
}

int beamProperties::getTDLnumber() const {
	return TDLnumber;
}



void ComputeSupports::PointLoadSupports() {			//determine the support value for each instance.
	for (int i = 0; i < PointLoadNumber; i++) {
		RightSupports.push_back((-(PointLoadLocationVect[i] - SupportLocationArray[0])*PointLoadMagnitudeVect[i]) / (SupportLocationArray[1] - SupportLocationArray[0]));	//calcualte each instance of the superposition left support
		LeftSupports.push_back(((PointLoadLocationVect[i] - SupportLocationArray[1])*PointLoadMagnitudeVect[i]) / (SupportLocationArray[1] - SupportLocationArray[0]));	//calculates each instance of the superposition right support
	}

	for (int i = 0; i < LeftSupports.size(); i++) {
		cout << LeftSupports[i] << " left is left, rght is (for point load) " << RightSupports[i] << endl;
	}
}

void ComputeSupports::UDLSupports() {			//determine the support values for UDL.
	for (int j = 0; j < UDLnumber; j++) {								//this loop performs the computation for the supports.
		UDLLeftSupports.push_back((-(UDLlocation_endVect[j] - UDLlocation_beginVect[j])*UDLmagnitudeVect[j])*(SupportLocationArray[1] - (0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j]))) / SupportLocationArray[1]);
		UDLRightSupports.push_back(-(UDLlocation_endVect[j] - UDLlocation_beginVect[j])*(UDLmagnitudeVect[j])*(0.5*(UDLlocation_endVect[j] + UDLlocation_beginVect[j])) / SupportLocationArray[1]);
	}

	for (int i = 0; i < UDLLeftSupports.size(); i++) {
		cout << UDLLeftSupports[i] << " left is left, rght is (for UDL load) " << UDLRightSupports[i] << endl;
	}
}

void ComputeSupports::TDLSupports() {
	for (int i = 0; i < TDLnumber; i++) {
		if (TDLgradientVect[i] == 'e') {		//if the gradient is at the end location and the force is a negative value.
			TDLLeftSupports.push_back(-(TDLmagnitudeVect[i] * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(beamlength - TDLlocation_beginVect[i] - (0.6666666667*(TDLlocation_endVect[i] - TDLlocation_beginVect[i]))) / (2 * beamlength)));	// left support = w*b*e/2L
			TDLRightSupports.push_back(-(0.5*(TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(TDLmagnitudeVect[i]) - (TDLmagnitudeVect[i] * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(beamlength - TDLlocation_beginVect[i] - (0.6666*(TDLlocation_endVect[i] - TDLlocation_beginVect[i]))) / (2 * beamlength))));		//compute right supports.
		}

		if (TDLgradientVect[i] == 'b') {		//down distributed loading, with the edge of triangle at the beginning location.
			TDLLeftSupports.push_back(-(0.5*(TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(TDLmagnitudeVect[i]) - (TDLmagnitudeVect[i] * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(beamlength - TDLlocation_beginVect[i] - (0.6666*(TDLlocation_endVect[i] - TDLlocation_beginVect[i]))) / (2 * beamlength))));
			TDLRightSupports.push_back(-(TDLmagnitudeVect[i] * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])*(beamlength - TDLlocation_beginVect[i] - (0.6666666667*(TDLlocation_endVect[i] - TDLlocation_beginVect[i]))) / (2 * beamlength)));	// left support = w*b*e/2L
		}
	}

	for (int i = 0; i < TDLLeftSupports.size(); i++) {
		cout << TDLLeftSupports[i] << " left is left, rght is (for TDL load) " << TDLRightSupports[i] << endl;
	}
}



void ShearForces::PointLoadShearSetup() {			//this function computes each instance of the point load shear
	for (int i = 0; i < PointLoadNumber; i++) {
		PointForceVal.push_back(RightSupports[i]);					//pushes back the right support value
		PointForceVal.push_back(LeftSupports[i]);					//pushes back the left support value
		PointForceVal.push_back(PointLoadMagnitudeVect[i]);			//pushes back the point load magnitude vector
		PointForceLocal.push_back(SupportLocationArray[1]);
		PointForceLocal.push_back(SupportLocationArray[0]);
		PointForceLocal.push_back(PointLoadLocationVect[i]);
	}

	for (int i = 0; i < PointForceLocal.size(); i++) {								//sorting the store_location vetor and store_force vector
		PointLoadPair.push_back(make_pair(PointForceLocal[i], PointForceVal[i]));	//sorts location vector.
	}

	/*for (int i = 0; i < PointLoadPair.size(); i++) {
		cout << PointLoadPair[i].first << " <- local force-> " << PointLoadPair[i].second << endl;
	}*/

	for (int i = 0; i < PointLoadPair.size(); i += 3) {								// sorts the location of point forces according to each force element.
		if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
			swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
			swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
		}

		if (PointLoadPair[i + 1].first > PointLoadPair[i + 2].first) {
			swap(PointLoadPair[i + 2].first, PointLoadPair[i + 1].first);
			swap(PointLoadPair[i + 2].second, PointLoadPair[i + 1].second);
		}

		if (PointLoadPair[i].first > PointLoadPair[i + 1].first) {
			swap(PointLoadPair[i].first, PointLoadPair[i + 1].first);
			swap(PointLoadPair[i].second, PointLoadPair[i + 1].second);
		}
	}

	for (int i = 0; i < PointLoadPair.size(); i++) {
		//cout << "force number  " << i << endl;
		cout << PointLoadPair[i].first << " <- location of forces/supports || local force-> " << PointLoadPair[i].second << endl;

	}
}

void ShearForces::ComputePointShear() {//computes the shear forces for the point load forces.

	for (int i = 0; i < PointLoadPair.size(); i += 3) {
		PointShearVect.push_back(PointLoadPair[i].second);
		PointShearVect.push_back(PointLoadPair[i].second + PointLoadPair[i + 1].second);
	}

	//for (int i = 0; i < PointShearVect.size(); i++) {			//this loop computes the local shear forces. take note that each superpostion will only have two shear forces (since each superposition will have two supports + one poit force only.
	//	cout << PointShearVect[i] << "<-- ShearForces " << PointLoadPair[i + 1].first << endl;	//critical points at which the forces change values.	
	//}

	int shearsize = 0;			//initialize varialbe called shear size
	int increment = 0;			//checks the element of pointloadpair
	


	while (1) {			//this while loop produces the
		for (int h = 3; h < PointLoadPair.size(); h += 3) {
			if (increment == h - 1) {		//since we dont want to compute from the last element of the first force to the beginning of the second force.
				increment = h;				//immediately set the increment to the first force
			}
		}

	
		//cout << j << " " << shearsize << endl;

		for (double i = 0; i < PointLoadPair.size(); i += 3) {	//checks the initial length.
			if (increment == i && PointLoadPair[i].first != 0) {
				for (double j = 0; j <= PointLoadPair[i].first * 10; j++) {
					PointShearSuperposition.push_back(0);
					ShearLocationSuperposition.push_back(j / 10);
				}
			}
		}

		for (double k = (PointLoadPair[increment].first) * 10; k <= (PointLoadPair[increment + 1].first) * 10; k++) {		//for each point load pair location.
			ShearLocationSuperposition.push_back(k / 10);											//push back the location distance, incremented by 0.2
			PointShearSuperposition.push_back(PointShearVect[shearsize]);							//push back the shear force value.

			for (int i = 1; i < PointLoadPair.size(); i += 3) {										//inserts extra shear values due at the transition region of each individual superpostion forces.
				if (k == PointLoadPair[i].first * 10) {				//if k is a transition location but also not the last location.			
					ShearLocationSuperposition.push_back(k / 10);
					PointShearSuperposition.push_back(PointShearVect[shearsize]);
				}
			}
		}

		/*for (double i = 2; i < PointLoadPair.size(); i += 3) {					//loop to insert zeroes in the last length.
			if ((increment + 1) == i && PointLoadPair[increment + 1].first <= beamlength) {
				for (double j = PointLoadPair[increment + 1].first * 10; j <= beamlength * 10; j++) {
					/*for (int k = 2; k < PointLoadPair.size(); k += 3) {			//checks if there are other trnasitonal regions after the maximum location.
						if (j == PointLoadPair[k].first*10) {
							PointShearSuperposition.push_back(0);
							ShearLocationSuperposition.push_back(j / 10);
						}
					}
					PointShearSuperposition.push_back(0);
					ShearLocationSuperposition.push_back(j / 10);
				}
			}
		}*/

		increment = increment + 1;
		shearsize += 1;
		if (shearsize >= PointShearVect.size()) {
			break;
		}

	}
	for (int i = 1; i < ShearLocationSuperposition.size() - 1; i++) {
		if ((PointShearSuperposition[i - 1] != PointShearSuperposition[i]) && (ShearLocationSuperposition[i] == ShearLocationSuperposition[i - 1])) {	//if the force value is different and the location is the same, provided that the location is not the first or lat location.
			//cout << PointShearSuperposition[i - 1] << " chosen values" << PointShearSuperposition[i] << " chosen values"  << ShearLocationSuperposition[i] << " chosen values"  << ShearLocationSuperposition[i - 1] << endl;
			for (int j = 1; j < PointLoadPair.size(); j += 3) {
				if ((ShearLocationSuperposition[i] == PointLoadPair[j].first) && (ShearLocationSuperposition[i - 1] == PointLoadPair[j].first)) { //if the loads are not the beginning and end loads
					cout << PointLoadPair[j].first << endl;
					//cout << PointShearSuperposition[i - 1] << " delete these " << PointShearSuperposition[i] << " delete these " << ShearLocationSuperposition[i] << " delete these " << ShearLocationSuperposition[i - 1] << endl;
					removeShear.push_back(i);
				}
			}
			//cout << PointShearSuperposition[i - 1] << " " << PointShearSuperposition[i] << " " << ShearLocationSuperposition[i] << " " << ShearLocationSuperposition[i - 1] << endl;
		}
	}

	for (int j = 0; j < removeShear.size(); j++) {		//thi loop is rsponsible fo removing assitional shear forces caused by the addtion of shear forces.
		PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - element);
		PointShearSuperposition.erase(PointShearSuperposition.begin() + removeShear[j] - 1 - element);
		ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - element);
		ShearLocationSuperposition.erase(ShearLocationSuperposition.begin() + removeShear[j] - 1 - element);
		element += 2;			//elements is needed. because the first two shears are removed, the element is decreased by 2.
	}

	for (int i = 0; i < ShearLocationSuperposition.size(); i++) {
		cout << ShearLocationSuperposition[i] << " <--- Location Plot || Shear value Superposition --> " << PointShearSuperposition[i] << endl;

	}

	const int SuperPositionSize = ShearLocationSuperposition.size();

	for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) {	//pushes back the first force values into the final shear force array
		PointShearFinal.push_back(PointShearSuperposition[i]);
	}

	for (int i = 0; i < PointShearFinal.size(); i++) {
		ShearLocationFinal.push_back(ShearLocationSuperposition[i]);
	}

	storeLastLocation = ShearLocationFinal[PointShearFinal.size() - 1];		//stores the last lcoation of the shear force (this variable is used to add zeroes if the last lcoation != beamlength.

	if (PointLoadNumber != 1) {					//if there is more than one point load, the superposition vector is split into parts and added to each index.
		for (int k = 1; k < PointLoadNumber; k++) {		//for each each instance of point load.
			for (int i = 0; i <= (ShearLocationSuperposition.size() / PointLoadNumber) - 1; i++) { //this loop sums all the point loads together.
				PointShearFinal[i] += PointShearSuperposition[k*(ShearLocationSuperposition.size() / PointLoadNumber) + i];	//sums the superposition forces together
			}
		}
	}

	/*if (ShearLocationFinal[0] != 0) {										//if the initial location is not zero, fill the beam with zero values and forces.
		for (double i = 0*10; i <= ShearLocationFinal[0]*10; i++) {
			ShearLocationFinal.push_back(i / 10);
			PointShearFinal.push_back(0);
		}
	}
	if (storeLastLocation <= beamlength) {		// if the final locationis not the beamlength.
		cout << storeLastLocation << endl;
		for (double i = storeLastLocation*10 + 2; i <= beamlength*10; i++) {
			ShearLocationFinal.push_back(i / 10);
			PointShearFinal.push_back(0);
		}
		ShearLocationFinal.push_back(storeLastLocation + 0.000001);		//since the sorting is unstable.
		PointShearFinal.push_back(0);
	}*/

	for (int i = 0; i < PointShearFinal.size(); i++) {			//create a pair vector for the final shear location vector and the final shear force vector
		PointShearFinalPair.push_back(make_pair(ShearLocationFinal[i], PointShearFinal[i]));
	}

	for (int i = 0; i < PointShearFinalPair.size() - 1; i++) {			//bubble sort to sort the vector,since the sort function in an unstable algo.
		for (int j = 0; j < PointShearFinalPair.size() - 1; j++) {
			if (PointShearFinalPair[j].first > PointShearFinalPair[j + 1].first) {		//if the previous element is greater than the later element.
				double temp = PointShearFinalPair[j + 1].first;
				PointShearFinalPair[j + 1].first = PointShearFinalPair[j].first;
				PointShearFinalPair[j].first = temp;
			}
		}
	}

	for (int i = 0; i < PointShearFinalPair.size(); i++) {
		cout << PointShearFinalPair[i].first << "<----LOCATION || SHEAR VALUE FINAL ---->" << PointShearFinalPair[i].second << endl;
	}
}

void ShearForces::computeUDLShear() {

	if (PointLoadNumber > 0) {
		for (int i = 1; i <= UDLnumber; i++) {			//this loop pushes back the required locations for the UDL location for each instance, provided that there was a presence of a point force. (since point shears contain the transition region).
			for (int j = 0; j < PointShearFinalPair.size(); j++) {
				UDLSuperpositionLocation.push_back(PointShearFinalPair[j].first);
			}
		}
	}
	else {
		for (int i = 1; i <= UDLnumber; i++) {							//for each UDL instance
			for (double j = 0 * 10; j <= beamlength * 10; j++) {		//push back values of 0 to the beam's length at increments of 0.2
				UDLSuperpositionLocation.push_back(j / 10);
			}
		}
	}

	for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {			//prints out superpostion lcoation values.
		cout << UDLSuperpositionLocation[i] << endl;
	}

	for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {			//intiialize superposition shear forces to zero.
		UDLSuperpositionShear.push_back(0);
	}

	for (int i = 0; i < UDLlocation_beginVect.size(); i++) {
		cout << UDLlocation_beginVect[i] << " <- begin" << UDLlocation_endVect[i] << "<- end" << UDLLeftSupports[i] << "<-- left supp" << UDLRightSupports[i] << endl;
	}

	int increment_location = 0, increment_size = (UDLSuperpositionLocation.size() / UDLnumber) - 1;
	int counter = 1;

	//for each instance of UDL load, compute each superposition shear
	cout << "test" << endl;
	cout << increment_location << endl;
	cout << increment_size << endl;
	cout << UDLSuperpositionLocation.size() << endl;
	for (int i = 0; i < UDLlocation_beginVect.size(); i++) {		//for each instance of UDL magnitude

		for (int j = increment_location; j <= increment_size; j++) {

			if (UDLSuperpositionLocation[j] <= UDLlocation_beginVect[i]) {		//while the location is smaller of equalto the the begining value of the UDL.
				cout << i << endl;
				UDLSuperpositionShear[j] = UDLLeftSupports[i];						//set the shear V be the left support reaction value
			}

			if ((UDLSuperpositionLocation[j] > UDLlocation_beginVect[i]) && (UDLSuperpositionLocation[j] < UDLlocation_endVect[i])) {	//if it is at the region of UDL
				UDLSuperpositionShear[j] = UDLLeftSupports[i] + UDLmagnitudeVect[i] * (UDLSuperpositionLocation[j] - UDLlocation_beginVect[i]);		// V = R + w(x - beginning location), where by w can be both postiive or negative.
			}

			if (UDLSuperpositionLocation[j] >= UDLlocation_endVect[i]) {		//after the UDL
				UDLSuperpositionShear[j] = -UDLRightSupports[i];				//V = -supportreaction

			}
			if (increment_location < (UDLSuperpositionLocation.size() - 1) && increment_size < (UDLSuperpositionLocation.size() - 1)) {	//prevent segfault
				increment_location += (UDLSuperpositionLocation.size() / UDLnumber);		//increment the initial superpositino lcoation
				increment_size += (UDLSuperpositionLocation.size() / UDLnumber);			//increment the the last element for each superposition
				cout << increment_location << endl;
				cout << increment_size << endl;
			}
		}

	}

	for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {
		cout << UDLSuperpositionLocation[i] << "<-- UDL SUPERPOSITION LOCATION || UDL SUPERPOSITION  SHEAR-->" << UDLSuperpositionShear[i] << endl;
	}

	for (int i = 0; i < (UDLSuperpositionLocation.size() / UDLnumber); i++) {		//this loop initializes the final location and shear values.
		UDLLocationFinal.push_back(UDLSuperpositionLocation[i]);
		UDLShearFinal.push_back(UDLSuperpositionShear[i]);
	}

	for (int k = 1; k < UDLnumber; k++) {			//this loop adds all the superposition values into a final value.
		for (int i = 0; i <= (UDLSuperpositionLocation.size() / UDLnumber) - 1; i++) {
			UDLShearFinal[i] += UDLSuperpositionShear[k*(UDLSuperpositionLocation.size() / UDLnumber) + i];
		}
	}

	for (int i = 0; i < UDLShearFinal.size(); i++) {			//prints out the 
		cout << UDLLocationFinal[i] << "<--- final Uniform distributed location|| final UDL shear --->" << UDLShearFinal[i] << endl;
	}
}

void ShearForces::computeTDLShear() {
	if (PointLoadNumber > 0) {
		for (int i = 1; i <= TDLnumber; i++) {			//this loop pushes back the required locations for the UDL location for each instance, provided that there was a presence of a point force. (since point shears contain the transition region).
			for (int j = 0; j < PointShearFinalPair.size(); j++) {
				TDLSuperpositionLocation.push_back(PointShearFinalPair[j].first);
			}
		}
	}
	else {
		for (int i = 1; i <= TDLnumber; i++) {
			for (double j = 0 * 10; j <= beamlength * 10; j++) {		//push back values of 0 to the beam's length at increments of 0.2
				TDLSuperpositionLocation.push_back(j / 10);
			}
		}
	}

	for (int i = 0; i < TDLSuperpositionLocation.size(); i++) {			//intiialize superposition shear forces to zero.
		TDLSuperpositionShear.push_back(0);
	}

	int increment_location_TDL = 0, increment_size_TDL = (TDLSuperpositionLocation.size() / TDLnumber) - 1;
	int counter_TDL = 1;

	for (int i = 0; i < TDLlocation_beginVect.size(); i++) {		//for each instance of TDL magnitude

		for (int j = increment_location_TDL; j <= increment_size_TDL; j++) {	//for each location of EACH superposition

			if (TDLSuperpositionLocation[j] <= TDLlocation_beginVect[i]) {		//while the location is smaller of equalto the the begining value of the UDL.
				TDLSuperpositionShear[j] = TDLLeftSupports[i];						//set the shear V be the left support reaction value
			}

			if ((TDLSuperpositionLocation[j] > TDLlocation_beginVect[i]) && (TDLSuperpositionLocation[j] < TDLlocation_endVect[i])) {	//if it is at the region of UDL
				if (TDLgradientVect[i] == 'e') {				//if the gradient is at the end
					TDLSuperpositionShear[j] = TDLLeftSupports[i] + (TDLmagnitudeVect[i] * (TDLSuperpositionLocation[j] - TDLlocation_beginVect[i]) * (TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])) / (2 * (TDLlocation_endVect[i] - TDLlocation_beginVect[i]));
				}	// V = R + w(x - beginning location)^2/0.5*(end - begin), where by w can be both postiive or negative.

				if (TDLgradientVect[i] == 'b') {			//if the gradient is at the beginning
					TDLSuperpositionShear[j] = TDLLeftSupports[i] + (TDLmagnitudeVect[i] * (TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])) - (TDLmagnitudeVect[i] * (TDLSuperpositionLocation[j] - TDLlocation_beginVect[i]) * (TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])) / (2 * (TDLlocation_endVect[i] - TDLlocation_beginVect[i]));
				}
			}

			if (TDLSuperpositionLocation[j] >= TDLlocation_endVect[i]) {		//after the UDL
				TDLSuperpositionShear[j] = -TDLRightSupports[i];				//V = -supportreaction
			}

			if (increment_location_TDL < (TDLSuperpositionLocation.size() - 1) && increment_size_TDL < (TDLSuperpositionLocation.size() - 1)) {	//prevent segfault
				increment_location_TDL += (TDLSuperpositionLocation.size() / TDLnumber);		//increment the initial superpositino lcoation
				increment_size_TDL += (TDLSuperpositionLocation.size() / TDLnumber);			//increment the the last element for each superposition
				cout << increment_location_TDL << endl;
				cout << increment_size_TDL << endl;
			}
		}
	}

	for (int i = 0; i < TDLSuperpositionLocation.size(); i++) {
		cout << TDLSuperpositionLocation[i] << "<-- TDL SUPERPOSITION LOCATION || TDL SUPERPOSITION  SHEAR-->" << TDLSuperpositionShear[i] << endl;
	}

	for (int i = 0; i < (TDLSuperpositionLocation.size() / TDLnumber); i++) {		//this loop initializes the final location and shear values.
		TDLLocationFinal.push_back(TDLSuperpositionLocation[i]);
		TDLShearFinal.push_back(TDLSuperpositionShear[i]);
	}

	for (int k = 1; k < TDLnumber; k++) {			//this loop adds all the superposition values into a final value.
		for (int i = 0; i <= (TDLSuperpositionLocation.size() / TDLnumber) - 1; i++) {
			TDLShearFinal[i] += TDLSuperpositionShear[k*(TDLSuperpositionLocation.size() / TDLnumber) + i];
		}
	}

	for (int i = 0; i < TDLShearFinal.size(); i++) {			//prints out the final shear force
		cout << TDLLocationFinal[i] << "<--- final TDL location|| final TDL shear --->" << TDLShearFinal[i] << endl;
	}



}

void ShearForces::ParseShearData() {			//parses data to excel
	if (PointLoadNumber > 0 && UDLnumber == 0 && TDLnumber == 0) {		//if there is only point load
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < PointShearFinal.size(); i++) {
			parsedata << PointShearFinalPair[i].first << "," << PointShearFinalPair[i].second << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber == 0 && UDLnumber > 0 && TDLnumber == 0) {		//if there is only UDL 
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < UDLShearFinal.size(); i++) {
			parsedata << UDLLocationFinal[i] << "," << UDLShearFinal[i] << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber == 0 && UDLnumber == 0 && TDLnumber > 0) {		//if there is only TDL 
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < TDLShearFinal.size(); i++) {
			parsedata << TDLLocationFinal[i] << "," << TDLShearFinal[i] << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber > 0 && UDLnumber > 0 && TDLnumber == 0) {		//point load + UDL
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < PointShearFinalPair.size(); i++) {
			parsedata << (UDLLocationFinal[i]) << "," << (PointShearFinalPair[i].second += UDLShearFinal[i]) << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber > 0 && UDLnumber == 0 && TDLnumber > 0) {		//point load + TDL
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < PointShearFinalPair.size(); i++) {
			parsedata << (TDLLocationFinal[i]) << "," << (PointShearFinalPair[i].second += TDLShearFinal[i]) << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber == 0 && UDLnumber > 0 && TDLnumber > 0) {		//TDL + UDL
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < PointShearFinalPair.size(); i++) {
			parsedata << UDLLocationFinal[i] << "," << (TDLShearFinal[i] += UDLShearFinal[i]) << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}

	if (PointLoadNumber > 0 && UDLnumber > 0 && TDLnumber > 0) {		//if all loads exists
		parsedata << "location" << "," << "Shear Force" << endl;
		parsedata << 0 << "," << 0 << endl;
		for (int i = 0; i < PointShearFinalPair.size(); i++) {
			parsedata << (UDLLocationFinal[i]) << "," << (PointShearFinalPair[i].second += (UDLShearFinal[i] + TDLShearFinal[i])) << endl;
		}
		parsedata << beamlength << "," << 0 << endl;
	}
}


void BendingMoments::ComputePointLoadMoments() {

	for (int i = 0; i < ShearLocationSuperposition.size(); i++) {		//intiialzie superpostion moments vector
		PointLoadSuperPositionMoments.push_back(0);
	}


	int pointshearsuperpositionlocation = 1; int pointshear_size = (ShearLocationSuperposition.size() / PointLoadNumber) - 1;

	for (int l = 1; l < PointLoadPair.size(); l += 3) {
		for (int i = pointshearsuperpositionlocation; i <= pointshear_size; i++) {			//loops through each superposition values.
			if ((ShearLocationSuperposition[i] == ShearLocationSuperposition[i - 1]) && (PointShearSuperposition[i] != PointShearSuperposition[i - 1]) && ShearLocationSuperposition[i] == PointLoadPair[l].first) {	//is the location is of the same value but different force value, i.e transition region.

				for (int w = pointshearsuperpositionlocation; w <= (i - 1); w++) {			//before trnasition region (first half)
					PointLoadSuperPositionMoments[w] = (ShearLocationSuperposition[w] * PointShearSuperposition[w]);	//computes the initial moments before the transtion region, M = V*x
				}

				for (int j = i; j < pointshear_size; j++) {						//compute the moment of the second half.
					PointLoadSuperPositionMoments[j] = ((ShearLocationSuperposition[j] * PointShearSuperposition[j]) - (PointShearSuperposition[j] * beamlength));		// M = mx + c, m = shear force, c = (-V*beamlength).
				}

				if (pointshearsuperpositionlocation < (ShearLocationSuperposition.size() - 1) && pointshear_size < (ShearLocationSuperposition.size() - 1)) {
					pointshearsuperpositionlocation += (ShearLocationSuperposition.size() / PointLoadNumber) - 1;
					pointshear_size += (ShearLocationSuperposition.size() / PointLoadNumber);
				}
			}
		}
	}


	for (int i = 0; i < PointLoadSuperPositionMoments.size(); i++) {
		cout << ShearLocationSuperposition[i] << "<-- location superpostion || moment superposition -->" << PointLoadSuperPositionMoments[i] << endl;
	}



	for (int i = 0; i <= (PointLoadSuperPositionMoments.size() / PointLoadNumber) - 1; i++) {	//pushes back the first moment values into the final moment
		PointMomentsFinal.push_back(PointLoadSuperPositionMoments[i]);
	}


	for (int k = 1; k < PointLoadNumber; k++) {		//for each each instance of point load.
		for (int i = 0; i <= (PointLoadSuperPositionMoments.size() / PointLoadNumber) - 1; i++) { //this loop sums all the point loads together.
			PointMomentsFinal[i] += PointLoadSuperPositionMoments[k*(ShearLocationSuperposition.size() / PointLoadNumber) + i];	//sums the superposition forces together
		}
	}

	for (int i = 0; i < PointMomentsFinal.size(); i++) {
		cout << PointShearFinalPair[i].first << " <-- FINAL LOCATION || FINAL MOMENTS --->" << PointMomentsFinal[i] << endl;
	}
}

void BendingMoments::computeUDLMoments() {							//this member function computes the bending moments for UDL.
	for (int i = 0; i < UDLSuperpositionLocation.size(); i++) {		//initialize the moments vector to zero.
		UDLSuperPositionMoments.push_back(0);						
	}

	int increment_momentslocation_UDL = 0;
	int increment_momentssize_UDL = (UDLSuperpositionLocation.size() / UDLnumber) - 1;

	for (int i = 0; i < UDLlocation_beginVect.size(); i++) {						//for each instance of UDL
		for (int j = increment_momentslocation_UDL; j <= increment_momentssize_UDL; j++) {	//loop for each superposition instance.
			if (UDLSuperpositionLocation[j] <= UDLlocation_beginVect[i]) {			//while the location is smaller or equal to the the begining value of the UDL.
				UDLSuperPositionMoments[j] = UDLLeftSupports[i] * UDLSuperpositionLocation[j];						//set the shear V be the left support reaction value, M = Vx
			}

			if ((UDLSuperpositionLocation[j] > UDLlocation_beginVect[i]) && (UDLSuperpositionLocation[j] < UDLlocation_endVect[i])) {	//if it is at the region of UDL
				UDLSuperPositionMoments[j] = (UDLLeftSupports[i] * UDLSuperpositionLocation[j]) + (UDLmagnitudeVect[i] * 0.5 * (UDLSuperpositionLocation[j] - UDLlocation_beginVect[i])*(UDLSuperpositionLocation[j] - UDLlocation_beginVect[i]));		// V = R + w(x - beginning location), where by w can be both postiive or negative.
			}

			if (UDLSuperpositionLocation[j] >= UDLlocation_endVect[i]) {		//after the UDL
				UDLSuperPositionMoments[j] = UDLRightSupports[i] * (beamlength - UDLSuperpositionLocation[j]);				//V = -supportreaction

			}
			if (increment_momentslocation_UDL < (UDLSuperpositionLocation.size() - 1) && increment_momentssize_UDL < (UDLSuperpositionLocation.size() - 1)) {	//prevent segfault, 
				increment_momentslocation_UDL += (UDLSuperpositionLocation.size() / UDLnumber);		//increment the initial superpositino lcoation to compute the next superposition value.
				increment_momentssize_UDL += (UDLSuperpositionLocation.size() / UDLnumber);			//increment the the last element for each superposition	to compute the next superposition component.
				//cout << increment_momentslocation << endl;
				//cout << increment_momentssize << endl;
			}
		}
	}

	for (int i = 0; i < UDLSuperPositionMoments.size(); i++) {			//prints the superposition values.
		cout << UDLSuperpositionLocation[i] << "<-- SuperPositionLocation TDL || Superposition moments TDL -->" << UDLSuperPositionMoments[i] << endl;
	}

	for (int i = 0; i < (UDLSuperPositionMoments.size() / UDLnumber); i++) {		//this loop initializes the final location and shear values.
		UDLMomentsFinal.push_back(UDLSuperPositionMoments[i]);
	}

	for (int k = 1; k < UDLnumber; k++) {			//this loop adds all the superposition values into a final value.
		for (int i = 0; i <= (UDLSuperPositionMoments.size() / UDLnumber) - 1; i++) {
			UDLMomentsFinal[i] += UDLSuperPositionMoments[k*(UDLSuperPositionMoments.size() / UDLnumber) + i];
		}
	}
}

void BendingMoments::computeTDLMoments() {
	for (int i = 0; i < TDLSuperpositionLocation.size(); i++) {		//initialize the superposition moments to zero.
		TDLSuperPositionMoments.push_back(0);
	}
	
	int increment_momentslocation_TDL = 0;
	int increment_momentssize_TDL = (TDLSuperpositionLocation.size() / TDLnumber) - 1;

	for (int i = 0; i < TDLlocation_beginVect.size(); i++) {						//for each instance of TDL
		for (int j = increment_momentslocation_TDL; j <= increment_momentssize_TDL; j++) {	//loop for each superposition instance.
			if (TDLSuperpositionLocation[j] <= TDLlocation_beginVect[i]) {			//while the location is smaller or equal to the the begining value of the UDL.
				TDLSuperPositionMoments[j] = TDLLeftSupports[i] * TDLSuperpositionLocation[j];						//set the shear V be the left support reaction value, M = Vx
			}

			if ((TDLSuperpositionLocation[j] > TDLlocation_beginVect[i]) && (TDLSuperpositionLocation[j] < TDLlocation_endVect[i])) {	//if it is at the region of TDL
				if (TDLgradientVect[i] == 'e') {
					TDLSuperPositionMoments[j] = ((TDLLeftSupports[i] * TDLSuperpositionLocation[j]) + ((TDLmagnitudeVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])) / (6 * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])));		// V = Rx + w(x - beginning location)^3/ (6*end - begin), where by w can be both postiive or negative.
				}

				if (TDLgradientVect[i] == 'b') {
					TDLSuperPositionMoments[j] = ((TDLLeftSupports[i] * TDLSuperpositionLocation[j]) + (0.5*TDLmagnitudeVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i]) - ((TDLmagnitudeVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])*(TDLSuperpositionLocation[j] - TDLlocation_beginVect[i])) / (6 * (TDLlocation_endVect[i] - TDLlocation_beginVect[i])));	//Rx + w(x - begin) - w*(x - beginning location)^3/ (6*end - begin)
				}
			}

			if (TDLSuperpositionLocation[j] >= TDLlocation_endVect[i]) {		//after the UDL
				TDLSuperPositionMoments[j] = TDLRightSupports[i] * (beamlength - TDLSuperpositionLocation[j]);				//V = -supportreaction
			}

			if (increment_momentslocation_TDL < (TDLSuperpositionLocation.size() - 1) && increment_momentssize_TDL < (TDLSuperpositionLocation.size() - 1)) {	//prevent segfault, 
				increment_momentslocation_TDL += (TDLSuperpositionLocation.size() / TDLnumber);		//increment the initial superpositino lcoation to compute the next superposition value.
				increment_momentssize_TDL += (TDLSuperpositionLocation.size() / TDLnumber);			//increment the the last element for each superposition	to compute the next superposition component.
			}
		}
	}

	for (int i = 0; i < (TDLSuperPositionMoments.size() / TDLnumber); i++) {		//this loop initializes the final location and shear values.
		TDLMomentsFinal.push_back(TDLSuperPositionMoments[i]);
	}

	for (int k = 1; k < TDLnumber; k++) {			//this loop adds all the superposition values into a final value.
		for (int i = 0; i <= (TDLSuperPositionMoments.size() / TDLnumber) - 1; i++) {
			TDLMomentsFinal[i] += TDLSuperPositionMoments[k*(TDLSuperPositionMoments.size() / TDLnumber) + i];
		}
	}
}

void BendingMoments::ParseMomentsData() {
	if (PointLoadNumber > 0 && UDLnumber == 0 && TDLnumber == 0) {		//if there is only point load
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < PointShearFinal.size(); i++) {
			parsedata << "," << "," << "," << PointShearFinalPair[i].first << "," << PointMomentsFinal[i] << endl;
		}
	}

	if (PointLoadNumber == 0 && UDLnumber > 0 && TDLnumber == 0) {		//if there is only UDL 
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < UDLShearFinal.size(); i++) {
			parsedata << "," << "," << "," << UDLLocationFinal[i] << "," << UDLMomentsFinal[i] << endl;
		}

	}

	if (PointLoadNumber == 0 && UDLnumber == 0 && TDLnumber > 0) {		//if there is only TDL
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < TDLShearFinal.size(); i++) {
			parsedata << "," << "," << "," << TDLLocationFinal[i] << "," << TDLMomentsFinal[i] << endl;
		}
	}

	if (PointLoadNumber > 0 && UDLnumber > 0 && TDLnumber == 0) {		//point + UDL
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < UDLLocationFinal.size(); i++) {
			parsedata << "," << "," << "," << (UDLLocationFinal[i]) << "," << (PointMomentsFinal[i] += UDLMomentsFinal[i]) << endl;
		}
	}

	if (PointLoadNumber > 0 && UDLnumber == 0 && TDLnumber > 0) {		//point + TDL
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < TDLLocationFinal.size(); i++) {
			parsedata << "," << "," << "," << (TDLLocationFinal[i]) << "," << (PointMomentsFinal[i] += TDLMomentsFinal[i]) << endl;
		}
	}

	if (PointLoadNumber == 0 && UDLnumber > 0 && TDLnumber > 0) {		//UDL + TDL
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < TDLLocationFinal.size(); i++) {
			parsedata << "," << "," << "," << (TDLLocationFinal[i]) << "," << (UDLMomentsFinal[i] += TDLMomentsFinal[i]) << endl;
		}
	}

	if (PointLoadNumber > 0 && UDLnumber > 0 && TDLnumber > 0) {		//all 3 loads present
		parsedata << "," << "," << "," << "location" << "," << "bending moments" << endl;
		for (int i = 0; i < TDLLocationFinal.size(); i++) {
			parsedata << "," << "," << "," << (TDLLocationFinal[i]) << "," << (PointMomentsFinal[i] += (UDLMomentsFinal[i] + TDLMomentsFinal[i])) << endl;
		}
	}

}



