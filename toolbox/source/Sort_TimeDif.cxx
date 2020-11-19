
#include "Sort_TimeDif.h"

using namespace std;

void Sort_TimeDif ( string filenames,string namefile,string treename = "god",int minanalog = -440,int maxanalog = -380,int mindigital = -220, int maxdigital = -180 )
{

    vector<string> rootfiles = DecodeItemsToTreat ( filenames, "system" );

    if ( rootfiles.size() ==0 )
    {
        return;
    }

    TFile* firstRootFile = new TFile ( rootfiles[0].c_str(), "read" );
    GoddessGeomInfos* geominfo = ( GoddessGeomInfos* ) firstRootFile->FindObjectAny ( "GoddessGeom" );
    GoddessReacInfos* reacinfos = ( GoddessReacInfos* ) firstRootFile->FindObjectAny ( "GoddessReac" );

    if ( geominfo == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information about the geometry are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacinfos == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information reaction are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    firstRootFile->Close();


    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );

    for ( int file =0; file < rootfiles.size(); file++ )
    {
        chain->Add ( rootfiles[file].c_str() );
    }

    int entries = chain->GetEntries();

    gam_struct gammas;
    //vector<si_struct> sistuffs;

    vector<SiDataBase>* sidata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si", &sidata );

    vector<GamData>* gamdata = new vector<GamData>;
    chain->SetBranchAddress ( "gam",&gamdata );
    
    vector<SiDataBase>* sid2 = new vector<SiDataBase>;

//     vector<vector<SiDataBase>>* new_si_data_set = new vector<vector<SiDataBase>>;
    //vector<GamData>* new_gam_data_set = new vector<GamData>;

    TFile* newfile = new TFile ( namefile.c_str(),"recreate" );
    TTree* newtree = new TTree ( "goddess","goddess" );
    newtree->Branch ( "si", &sid2 );
    newtree->Branch ( "gam",&gammas );

//     newtree->Write();

    long long int dif = 0;
    long long unsigned int sit = 0;
    long long unsigned int gt = 0;
    float b = 0;
    float pos = 0;
    
    for (int p = 0; p<2; p++)
    {
      gammas.gam_energy.push_back(0);
      gammas.crystal_num.push_back(0);
    }

    for ( int i = 0; i<entries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%200000 == 0 )
        {
            cout<<"Entries Treated: "<<i<<"/"<<entries<<"\r"<<flush;

        }
// 
//         cout<<sidata->size()<<endl;
// 
        if ( sidata->size() > 0 && gamdata->size() >0 && gamdata->size() <=2)
        {
	  
	gammas.gam_energy.at ( 0 ) = -1;
        gammas.crystal_num.at ( 0 ) = -1;
        gammas.gam_energy.at ( 1 ) = -1;
        gammas.crystal_num.at ( 1 ) = -1;
	gammas.time = -1;
// 
	
	SiDataBase* sb = &sidata->at(0);
	SiDataBase* sd;
	b = sb->ESumLayer();
	pos = sb->PosE1().Mag();
// 	cout<<b<<endl;
	GamData* gamevent = &gamdata->at ( 0 );
		  
	    if (gamevent->type != 1) continue;

            for ( int j = 0; j<sidata->size(); j++ )
            {
	      
                SiDataBase* sievent = &sidata->at ( j );
		
		  //GamData* gamevent = &gamdata->at ( 0 );
		  
		  //if (gamevent->type != 1) continue;
		  

		    dif = 0;
                    sit = sievent->TimestampMaxLayer();
		    gt = gamevent->time;
		    
		    //dif = gt - sit;
		    dif = sit - gt;

                    if ( sievent->isBarrel && sievent->isUpstream )
                    {
                        if ( sievent->sector==0 || sievent->sector > 6 )
                        {

                            if ( dif < maxanalog && dif > minanalog )
                            {
			      
// 			      cout<<"third if"<<endl;
		sd = &sidata->at(j);
		sid2->push_back(*sd);
			      
//                                     sistuffs.at(j).isUpstream = ( int ) sievent->isUpstream;
//                                     sistuffs.at(j).isBarrel = ( int ) sievent->isBarrel;
//                                     sistuffs.at(j).sector_strip = sievent->sector;
//                                     sistuffs.at(j).sector_strip += sievent->StripMaxLayer ( 1, false ) * 0.01;
//                                     sistuffs.at(j).energy = sievent->ESumLayer ( 1, false );
//                                     sistuffs.at(j).pos = sievent->PosE1();
// 				    sistuffs.at(j).time = sievent->TimestampMaxLayer(1,false);
// 				    sistuffs.at(j).angle = sievent->Angle(1);
// 				    sistuffs.at(j).mult = sievent->MultLayer();
// 				    
// 				    b = sistuffs.at(j).energy;
// 				    
// 				    if (sistuffs.at(j).sector_strip == -1 || sistuffs.at(j).energy == -1 || sistuffs.at(j).time == -1 || sistuffs.at(j).angle == -1) continue;
// 				    
// 				    cout<<sistuffs.energy<<endl;
// 				    
// 
//                                         gammas.gam_energy[0] = gamevent->en; //energy is divided by 3 for Xe but not for Mo
//                                         gammas.crystal_num[0] = gamevent->num;
// 					gammas.time = gamevent->time;
// 					
// 					if (gammas.gam_energy[0] == -1 || gammas.crystal_num[0] == -1 || gammas.time == -1) continue;
// 					
// 					if (gamdata->size()==2 && gammas.gam_energy[1] != -1)
// 					{
// 					gammas.gam_energy[1]=gamdata->at(1).en;
// 					gammas.crystal_num[1] = gamdata->at(1).num;
// 					}

                            }
                            else continue;
                            
//                             if ( sistuffs.energy > 0 && sistuffs.pos.Mag() > 0 )
// 				{
// 				    newtree->Fill();
// 				    //cout<<outtree<<endl;
// 				  }
                        }
                        else if ( sievent->sector<6 && sievent->sector != 0 )
                        {
                            if ( dif < maxdigital && dif > mindigital )
                            {
//                                     if ( b != sievent->ESumLayer(1,false) )
//                                 {
				
		//sd->Clear();
		sd = &sidata->at(j);
		sid2->push_back(*sd);
				  
//                                     sistuffs.at(j).isUpstream = ( int ) sievent->isUpstream;
//                                     sistuffs.at(j).isBarrel = ( int ) sievent->isBarrel;
//                                     sistuffs.at(j).sector_strip = sievent->sector;
//                                     sistuffs.at(j).sector_strip += sievent->StripMaxLayer ( 1, false ) * 0.01;
//                                     sistuffs.at(j).energy = sievent->ESumLayer ( 1, false );
//                                     sistuffs.at(j).pos = sievent->PosE1();
// 				    sistuffs.at(j).time = sievent->TimestampMaxLayer(1,false);
// 				    sistuffs.at(j).angle = sievent->Angle(1);
// 				    sistuffs.at(j).mult = sievent->MultLayer();
// 				    
// 				    b = sistuffs.at(j).energy;
// 				    
// 				    if (sistuffs.at(j).sector_strip == -1 || sistuffs.at(j).energy == -1 || sistuffs.at(j).time == -1 || sistuffs.at(j).angle == -1) continue;
//                                     if ( gamevent->type == 1 )
//                                     {
//                                         gammas.gam_energy[0] = gamevent->en; //energy is divided by 3 for Xe but not for Mo
//                                         gammas.crystal_num[0] = gamevent->num;
// 					gammas.time = gamevent->time;
// 					
// 					if (gamdata->size()==2)
// 					{
// 					  gammas.gam_energy[1]=gamdata->at(1).en;
// 					gammas.crystal_num[1] = gamdata->at(1).en;
// 					}
                                    }
                                
                                else continue;

                            }
                            
//                             if ( sistuffs.energy > 0 && sistuffs.pos.Mag() > 0 )
// 				{
// 				    newtree->Fill();
// 				    //cout<<outtree<<endl;
// 				  }
//                         }

//                         cout<<"After Digital SX3"<<endl;

                    }
                    else if ( sievent->isUpstream && !sievent->isBarrel )
                    {
                        if ( sievent->sector == 0 || sievent->sector==1 )
                        {
                            if ( dif < maxdigital && dif > mindigital )
                            {
//                                if ( b != sievent->ESumLayer(1,false) )
//                                 {
		//sd->Clear();
		sd = &sidata->at(j);
		sid2->push_back(*sd);
//                                      sistuffs.at(j).isUpstream = ( int ) sievent->isUpstream;
//                                     sistuffs.at(j).isBarrel = ( int ) sievent->isBarrel;
//                                     sistuffs.at(j).sector_strip = sievent->sector;
//                                     sistuffs.at(j).sector_strip += sievent->StripMaxLayer ( 1, false ) * 0.01;
//                                     sistuffs.at(j).energy = sievent->ESumLayer ( 1, false );
//                                     sistuffs.at(j).pos = sievent->PosE1();
// 				    sistuffs.at(j).time = sievent->TimestampMaxLayer(1,false);
// 				    sistuffs.at(j).angle = sievent->Angle(1);
// 				    sistuffs.at(j).mult = sievent->MultLayer();
// 				    
// 				    b = sistuffs.at(j).energy;
// 				    
// 				    if (sistuffs.at(j).sector_strip == -1 || sistuffs.at(j).energy == -1 || sistuffs.at(j).time == -1 || sistuffs.at(j).angle == -1) continue;
// 				    
// 
//                                     if ( gamevent->type == 1 )
//                                     {
//                                         gammas.gam_energy[0] = gamevent->en; //energy is divided by 3 for Xe but not for Mo
//                                         gammas.crystal_num[0] = gamevent->num;
// 					gammas.time = gamevent->time;
// 					
// 					if (gamdata->size()==2)
// 					{
// 					  gammas.gam_energy[1]=gamdata->at(1).en;
// 					gammas.crystal_num[1] = gamdata->at(1).en;
// 					}
//                                     }
                                }
                                else continue;

                            }
                            
//                             if ( sistuffs.energy > 0 && sistuffs.pos.Mag() > 0 )
// 				{
// 				    newtree->Fill();
// 				    //cout<<outtree<<endl;
// 				  }
//                             cout<<"After digital QQQ5"<<endl;
                        else if ( sievent->sector == 2 || sievent->sector == 3 )
                        {
                            if ( dif < maxanalog && dif >minanalog )
                            {
//                                if ( b != sievent->ESumLayer(1,false) )
//                                 {
		//sd->Clear();
		sd = &sidata->at(j);
		sid2->push_back(*sd);
	
//                                     sistuffs.at(j).isUpstream = ( int ) sievent->isUpstream;
//                                     sistuffs.at(j).isBarrel = ( int ) sievent->isBarrel;
//                                     sistuffs.at(j).sector_strip = sievent->sector;
//                                     sistuffs.at(j).sector_strip += sievent->StripMaxLayer ( 1, false ) * 0.01;
//                                     sistuffs.at(j).energy = sievent->ESumLayer ( 1, false );
//                                     sistuffs.at(j).pos = sievent->PosE1();
// 				    sistuffs.at(j).time = sievent->TimestampMaxLayer(1,false);
// 				    sistuffs.at(j).angle = sievent->Angle(1);
// 				    sistuffs.at(j).mult = sievent->MultLayer();
// 				    
// 				    b = sistuffs.at(j).energy;
// 				    
// 				    if (sistuffs.at(j).sector_strip == -1 || sistuffs.at(j).energy == -1 || sistuffs.at(j).time == -1 || sistuffs.at(j).angle == -1) continue;
// 
//                                     if ( gamevent->type == 1 )
//                                     {
//                                         gammas.gam_energy[0] = gamevent->en; //energy is divided by 3 for Xe but not for Mo
//                                         gammas.crystal_num[0] = gamevent->num;
// 					gammas.time = gamevent->time;
// 					
// 					if (gamdata->size()==2)
// 					{
// 					  gammas.gam_energy[1]=gamdata->at(1).en;
// 					gammas.crystal_num[1] = gamdata->at(1).en;
// 					}
//                                     }
                                }
                                else continue;
			    }
		    }
		    else continue;
				
// 				if ( sistuffs.energy > 0 && sistuffs.pos.Mag() > 0 )
// 				{
// 				    newtree->Fill();
// 				    //cout<<outtree<<endl;
// 				  }

			
// 			new_si_data_set->push_back(sid2);
//                     }
//                     else continue;
		 
// 			cout<<sid2->size()<<endl;

		    
// 		    cout<<"p "<<sb->ESumLayer()<<endl;
                }

               gammas.gam_energy[0] = gamevent->en; //energy is divided by 3 for Xe but not for Mo
               gammas.crystal_num[0] = gamevent->num;
	       gammas.time = gamevent->time;
					
// 	       cout<<"g "<<gammas.gam_energy[0]<<endl;
	       
	      if (gammas.gam_energy[0] == -1 || gammas.crystal_num[0] == -1 || gammas.time == -1) continue;
					
	      if (gamdata->size()==2 && gammas.gam_energy[1] != -1)
		{
		  gammas.gam_energy[1]=gamdata->at(1).en;
		  gammas.crystal_num[1] = gamdata->at(1).num;
			}
//                 cout<<"Before switch."<<endl;

                if ( gammas.gam_energy[0] > gammas.gam_energy[1] )
                {
                    swap ( gammas.gam_energy[0], gammas.gam_energy[1] );
                    swap ( gammas.crystal_num[0], gammas.crystal_num[1] );
                }
                
                
                if (sid2->size()>0){
                
                if ( b> 0 && sb->PosE1().Mag()> 0 )
		  {
		    newtree->Fill();
// 				    cout<<outtree<<endl;
		      }
		}
		
		sid2->clear();
	

                
            }
            
            else continue;
            
        }
        
//     newtree->Fill();



    newtree->Write ( "goddess", TObject::kOverwrite );

    newfile->mkdir ( "infos" );
    newfile->cd ( "infos" );
    geominfo->Write ( "GoddessGeom" );
    reacinfos->Write ( "GoddessReac" );
    newfile->Close();


    return;
}


void QValvsAngle (string filenames,string outfile, string treename, bool isUpstream, bool isBarrel, float sector_strip_min, float sector_strip_max)
{
  
      vector<string> rootfiles = DecodeItemsToTreat ( filenames, "system" );

    if ( rootfiles.size() ==0 )
    {
        return;
    }

    TFile* firstRootFile = new TFile ( rootfiles[0].c_str(), "read" );
    GoddessGeomInfos* geominfo = ( GoddessGeomInfos* ) firstRootFile->FindObjectAny ( "GoddessGeom" );
    GoddessReacInfos* reacinfos = ( GoddessReacInfos* ) firstRootFile->FindObjectAny ( "GoddessReac" );

    if ( geominfo == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information about the geometry are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacinfos == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information reaction are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    firstRootFile->Close();


    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );
    
    TFile* f = new TFile(outfile.c_str(),"recreate");
    
    TH2F* histo = new TH2F("QValvsAngle","QVal vs Angle",1400,0,180,1400,-40,40);
    TH2F* histo2 = new TH2F("EnergyvsAngle","Energy vs Angle",1400,0,180,1400,0,14);
    
    float qval = 0;
    
    float energy = 0; 

    for ( int file =0; file < rootfiles.size(); file++ )
    {
        chain->Add ( rootfiles[file].c_str() );
    }

    int entries = chain->GetEntries();
    
    si_struct* sidata = new si_struct;
    chain->SetBranchAddress ( "si", &sidata );
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%200000 == 0 )
        {
            cout<<"Entries Treated: "<<entry<<"/"<<entries<<"\r"<<flush;

        }
        
//         for ( int j = 0; j<sidata->size(); j++ )
//             {
//                 si_struct* sievent = &sidata->at ( entry ); 
		
		if (sidata->isUpstream == isUpstream && sidata->isBarrel == isBarrel)
		{
		  if (sidata->sector_strip >= sector_strip_min && sidata->sector_strip <= sector_strip_max)
		  {
		    if (sidata->energy == energy) continue;
		
		qval = SiDataBase::QValue(reacinfos,sidata->energy,sidata->angle*TMath::DegToRad());
		histo->Fill(sidata->angle,reacinfos->qValGsGs-qval);
		histo2->Fill(sidata->angle, sidata->energy);
		
		  }
		}
// 	    }
      energy = sidata->energy;
    }
    
    
    histo->Write();
    histo2->Write();
    f->Write();
    f->Close();
    
    
  return;
}


void ReSort ( string filenames,string namefile,string treename = "god",int minanalog = -422,int maxanalog = -415,int mindigital = -200, int maxdigital = -180 )
{

    vector<string> rootfiles = DecodeItemsToTreat ( filenames, "system" );

    if ( rootfiles.size() ==0 )
    {
        return;
    }

    TFile* firstRootFile = new TFile ( rootfiles[0].c_str(), "read" );
    GoddessGeomInfos* geominfo = ( GoddessGeomInfos* ) firstRootFile->FindObjectAny ( "GoddessGeom" );
    GoddessReacInfos* reacinfos = ( GoddessReacInfos* ) firstRootFile->FindObjectAny ( "GoddessReac" );

    if ( geominfo == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information about the geometry are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacinfos == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information reaction are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    firstRootFile->Close();


    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );

    for ( int file =0; file < rootfiles.size(); file++ )
    {
        chain->Add ( rootfiles[file].c_str() );
    }

    int entries = chain->GetEntries();
    
    vector<SiDataBase> si_ev;
    vector <GamData> gam_ev; 
    
        vector<SiDataBase>* sidata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si", &sidata );

    vector<GamData>* gamdata = new vector<GamData>;
    chain->SetBranchAddress ( "gam",&gamdata );

    TFile* newfile = new TFile ( namefile.c_str(),"recreate" );
    TTree* newtree = new TTree ( "goddess","goddess" );
    newtree->Branch ( "si", &si_ev );
    newtree->Branch ( "gam",&gam_ev );

    newtree->Write();
    
    for (int en = 0; en<entries; en++)
    {
      
    }
    
    
    return;
}


