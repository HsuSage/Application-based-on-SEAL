/*
Date: 2016-07-15
Husen.wang@uni.lu 
University of Luxembourg & SnT
*/

#include "SPP.h"


void print_example_banner(string title)
{
    if (!title.empty())
    {
        size_t title_length = title.length();
        size_t banner_length = title_length + 2 + 2 * 10;
        string banner_top(banner_length, '*');
        string banner_middle = string(10, '*') + " " + title + " " + string(10, '*');

        cout << endl
            << banner_top << endl
            << banner_middle << endl
            << banner_top << endl
            << endl;
    }
}

EncryptionParameters SPP_Parameter_Gen()
{
    ChooserEncoder chooser_encoder;
    ChooserEvaluator chooser_evaluator;
    
    //input: Max poly size, max every coefficient size
    //7=x^2+x+1
    ChooserPoly Ib(1, 1);
    ChooserPoly qtb(1, 1); //q_{t,b}
    ChooserPoly wuf(5, 1);  
    ChooserPoly qfb(1, 1); //q_{f,b}
    ChooserPoly ruavg(3, 1); //r_u

    // Compute the first term
    //Strangers
    ChooserPoly test = chooser_evaluator.multiply_plain(Ib, qtb);

    ChooserPoly RIb = chooser_evaluator.multiply_plain(Ib, chooser_encoder.encode(1)); //*n*Rt, only one Ib element is 1
    ChooserPoly RIbs = chooser_evaluator.sub_plain(RIb, ruavg); //n*rt_avg
    ChooserPoly ntb = chooser_evaluator.multiply(RIbs, qtb); 

    ChooserPoly nT(1, 1);
    ChooserPoly dT(1, 1);
    for(int i=0; i<0.5*Tu; i++)  //set density to 0.5
    {
        nT=chooser_evaluator.add(nT, ntb); //Tu*
        dT=chooser_evaluator.add(dT, qtb); //Tu*
    }
    

    //Friends
    ChooserPoly Rfb = chooser_evaluator.multiply_plain(Ib, chooser_encoder.encode(1)); //*n*Rt
    ChooserPoly Rfbs = chooser_evaluator.sub_plain(Rfb, ruavg); //n*rt_avg
    ChooserPoly nfb = chooser_evaluator.multiply(Rfbs, qfb); 
    ChooserPoly nfbw = chooser_evaluator.multiply(nfb, wuf); 

    ChooserPoly nF(1, 1);
    ChooserPoly dF(1, 1);
    for(int i=0; i<0.5*Fu; i++)
    {
        nF=chooser_evaluator.add(nF, nfb); //Fu*
        dF=chooser_evaluator.add(dF, qfb); //Fu*
    }
    //X Y
    ChooserPoly X1 = chooser_evaluator.multiply(ruavg, dF);
    ChooserPoly X12 = chooser_evaluator.multiply(X1, dT);
    ChooserPoly X123 = chooser_evaluator.multiply_plain(X12, chooser_encoder.encode(alpha+beta));
    ChooserPoly X2 = chooser_evaluator.multiply(nT, dF);
    ChooserPoly X22 = chooser_evaluator.multiply_plain(X2, chooser_encoder.encode(beta));
    ChooserPoly X3 = chooser_evaluator.multiply(nF, dT);
    ChooserPoly X32 = chooser_evaluator.multiply_plain(X3, chooser_encoder.encode(alpha));

    //ChooserPoly Xs = chooser_evaluator.add(X123, X22);
    ChooserPoly X = chooser_evaluator.add(X22, X32);

    ChooserPoly Ys = chooser_evaluator.multiply(dF, dT);
    ChooserPoly Y = chooser_evaluator.multiply_plain(Ys, chooser_encoder.encode(alpha+beta));

    // To find an optimized set of parameters, we use ChooserEvaluator::select_parameters(...).
    EncryptionParameters optimal_parms;
    chooser_evaluator.select_parameters(X, optimal_parms);

    // Let's print these to see what was recommended
    cout << "Selected parameters:" << endl;
    cout << "{ poly_modulus: " << optimal_parms.poly_modulus().to_string() << endl;
    cout << "{ coeff_modulus: " << optimal_parms.coeff_modulus().to_string() << endl;
    cout << "{ plain_modulus: " << optimal_parms.plain_modulus().to_string() << endl; //to_dec_string
    cout << "{ decomposition_bit_count: " << optimal_parms.decomposition_bit_count() << endl;
    cout << "{ noise_standard_deviation: " << optimal_parms.noise_standard_deviation() << endl;
    cout << "{ noise_max_deviation: " << optimal_parms.noise_max_deviation() << endl;

    return optimal_parms;
}

//TDSC implementation
//Single Prediciton Protocol
void SPP( int user_num, int item_num, EncryptionParameters params2, HEsystem hesystem)
{
    print_example_banner("Implementation:Single Prediciton Protocol");

    //load users, strangers, friends, matrix
    int MTu[Tu][n], MFu[Fu][n], w[Fu];//modify, use new*
    LoadM(MTu, MFu, w, user_num);
    //terminate();

    std::clock_t start;    
    start=clock();
    
    ChooserEncoder chooser_encoder;
    ChooserEvaluator chooser_evaluator;
    
    //input: Max poly size, max every coefficient size
    //7=x^2+x+1
    ChooserPoly Ib(1, 1);
    ChooserPoly qtb(1, 1); //q_{t,b}
    ChooserPoly wuf(5, 1);  
    ChooserPoly qfb(1, 1); //q_{f,b}
    ChooserPoly ruavg(3, 1); //r_u

    // Compute the first term
    //Strangers
    ChooserPoly test = chooser_evaluator.multiply_plain(Ib, qtb);

    ChooserPoly RIb = chooser_evaluator.multiply_plain(Ib, chooser_encoder.encode(1)); //*n*Rt, only one Ib element is 1
    ChooserPoly RIbs = chooser_evaluator.sub_plain(RIb, ruavg); //n*rt_avg
    ChooserPoly ntb = chooser_evaluator.multiply(RIbs, qtb); 

    ChooserPoly nT(1, 1);
    ChooserPoly dT(1, 1);
    for(int i=0; i<0.5*Tu; i++)  //set density to 0.5
    {
        nT=chooser_evaluator.add(nT, ntb); //Tu*
        dT=chooser_evaluator.add(dT, qtb); //Tu*
    }
    

    //Friends
    ChooserPoly Rfb = chooser_evaluator.multiply_plain(Ib, chooser_encoder.encode(1)); //*n*Rt
    ChooserPoly Rfbs = chooser_evaluator.sub_plain(Rfb, ruavg); //n*rt_avg
    ChooserPoly nfb = chooser_evaluator.multiply(Rfbs, qfb); 
    ChooserPoly nfbw = chooser_evaluator.multiply(nfb, wuf); 

    ChooserPoly nF(1, 1);
    ChooserPoly dF(1, 1);
    for(int i=0; i<0.5*Fu; i++)
    {
        nF=chooser_evaluator.add(nF, nfb); //Fu*
        dF=chooser_evaluator.add(dF, qfb); //Fu*
    }
    //X Y
    ChooserPoly X1 = chooser_evaluator.multiply(ruavg, dF);
    ChooserPoly X12 = chooser_evaluator.multiply(X1, dT);
    ChooserPoly X123 = chooser_evaluator.multiply_plain(X12, chooser_encoder.encode(alpha+beta));
    ChooserPoly X2 = chooser_evaluator.multiply(nT, dF);
    ChooserPoly X22 = chooser_evaluator.multiply_plain(X2, chooser_encoder.encode(beta));
    ChooserPoly X3 = chooser_evaluator.multiply(nF, dT);
    ChooserPoly X32 = chooser_evaluator.multiply_plain(X3, chooser_encoder.encode(alpha));

    //ChooserPoly Xs = chooser_evaluator.add(X123, X22);
    ChooserPoly X = chooser_evaluator.add(X22, X32);

    ChooserPoly Ys = chooser_evaluator.multiply(dF, dT);
    ChooserPoly Y = chooser_evaluator.multiply_plain(Ys, chooser_encoder.encode(alpha+beta));

    // To find an optimized set of parameters, we use ChooserEvaluator::select_parameters(...).
    EncryptionParameters params;
    chooser_evaluator.select_parameters(X, params);

   	cout << "Generating keys..." << endl;
	KeyGenerator generator(params);
	generator.generate();
	cout << "... key generation complete" << endl;
	BigPoly public_key = generator.public_key();
	BigPoly secret_key = generator.secret_key();
	EvaluationKeys evaluation_keys = generator.evaluation_keys();

    // Create the encoding/encryption tools
    BalancedEncoder encoder(params.plain_modulus());
    Encryptor encryptor(params, public_key);
    Evaluator evaluator(params, evaluation_keys);
    Decryptor decryptor(params, secret_key);

    //Ttest(encryptor, decryptor,evaluator, encoder, start);

    // Now perform the computations on real encrypted data.
    int i,j;
    //stage1, strangers
    BigPoly EIb[n], RtIb, tmp, tmps, RtIb2; //Rt are plaintexts
    int Rtavg[Tu]; //plain qtb
    BigPoly Ent[Tu], Eqtb[Tu];

    //user
    start=clock();

    //user encryption, encrypt target item_num to 1
    encryptor.encrypt( encoder.encode(1), EIb[item_num] );
    for(i=0; i<n; i++)
        if(i!=item_num)
        encryptor.encrypt( encoder.encode(0), EIb[i] );
    cout<<"Time for stage1 user:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();

    //strangers encryption
    for(j=0; j<Tu; j++)
    {
        Rtavg[j]=0;
        for(i=0; i<n; i++)
            Rtavg[j]+=MTu[j][i];
        Rtavg[j]=Rtavg[j]/n;
    }

    for(j=0; j<Tu; j++)
    {
        int k=0;
        while((k<n)&&(MTu[j][k]==0)) 
			k++;

		//cout<<"k:"<<k<<endl;

        if(k==n) 
            encryptor.encrypt( encoder.encode(0), Eqtb[j] );
        else
            evaluator.add_plain(EIb[k], encoder.encode(0), Eqtb[j] );

        //cout<<"here0"<<endl;

        for(i=k+1; i<n; i++)
            if(MTu[j][i]!=0)
                Eqtb[j]=evaluator.add(Eqtb[j],EIb[i]);
    }
        
    cout<<"here1"<<endl;

    for(j=0; j<Tu; j++)
    {
    	//cout<<j<<endl;
        //Rt*Ib
        evaluator.multiply_plain(EIb[0], encoder.encode(MTu[j][0]), RtIb ); 
        //cout<<"here2"<<endl;
        for(i=1; i<n; i++)
        {      	 
            BigPoly tmp=evaluator.add(RtIb,  evaluator.multiply_plain(EIb[i], encoder.encode(MTu[j][i]) ));           
            RtIb=tmp;
        }
        //cout<<"here3"<<endl;
        evaluator.multiply(evaluator.sub_plain(RtIb, encoder.encode(Rtavg[j]) ), Eqtb[j], Ent[j] );
    }   
    cout<<"Time for stage1 Strangers:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();

    //stage2, friends
    //user encryption : similarity
    BigPoly Ew[Fu];
    for(i=0; i<Fu; i++) 
        encryptor.encrypt( encoder.encode(w[i] ), Ew[i] );

    cout<<"Time for stage2 user:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();

    //friends encryption
    BigPoly RfIb, RfIb2, Eqfb2; //Rt are plaintexts
    int Rfavg[Fu];
    BigPoly Enf[Fu], Eqfb[Fu];

    for(j=0; j<Fu; j++)
    {
        Rfavg[j]=0;
        for(i=0; i<n; i++)
            Rfavg[j]+=MFu[j][i];
        Rfavg[j]=Rfavg[j]/n;
    }

    for(j=0; j<Fu; j++)
        {
            int k=0;
            while((k<n)&&(MFu[j][k]==0))
            {           k++;
            } 
            

            if(k==n) 
                encryptor.encrypt( encoder.encode(0), Eqfb[j] );
            else
                evaluator.add_plain(EIb[k], encoder.encode(0), Eqfb[j] );

            for(i=k+1; i<n; i++)
            {
                if(MFu[j][i]!=0)
                        {
                            BigPoly tmp=evaluator.add(Eqfb[j],EIb[i]);
                            Eqfb[j]=tmp;
                        }
            }
        }

    for(j=0; j<Fu; j++)
    {
        evaluator.multiply_plain(EIb[0], encoder.encode(MFu[j][0]), RfIb ); 
        for(i=1; i<n; i++)
        {
            BigPoly tmp=evaluator.add(RfIb, evaluator.multiply_plain(EIb[i], encoder.encode(MFu[j][i]) ) );
            RfIb=tmp;
        }
        evaluator.multiply(evaluator.sub_plain(RfIb, encoder.encode(Rfavg[j]) ), evaluator.multiply(Eqfb[j], Ew[j] ),  Enf[j]);
    }   

    cout<<"Time for stage2 friends:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();

    //Stage 3
    cout<<"Time for stage3 user:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();

    //RS server
    //sum all nt
    BigPoly EnT=Ent[0], EdT=Eqtb[0];
    for(i=1; i<Tu; i++)
    {
        BigPoly tmp1=evaluator.add(EnT, Ent[i] );
        EnT=tmp1;

        BigPoly tmp2=evaluator.add(EdT, Eqtb[i] );        
        EdT=tmp2;
    }

    BigPoly EnF=Enf[0], EdF=Eqfb[0];
    for(i=1; i<Fu; i++)
    {
        BigPoly tmp1=evaluator.add(EnF, Enf[i] );
        EnF=tmp1;

        BigPoly tmp2=evaluator.add(EdF, Eqfb[i] );        
        EdF=tmp2;      
    }

    BigPoly EX=evaluator.add(evaluator.multiply_plain(evaluator.multiply(EdF, EnT), encoder.encode(beta) )  , evaluator.multiply_plain(evaluator.multiply(EdT, EnF), encoder.encode(alpha) )  );
    BigPoly EY=evaluator.multiply_plain(evaluator.multiply(EdT, EdF), encoder.encode(alpha+beta) ) ; 
    
    cout<<"Time for stage3 server:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    start=clock();


    // Finally print the result and noise
    cout << "EX: " << encoder.decode_int64(decryptor.decrypt(EX).to_string()) << ", ";
    cout << "EY: " << encoder.decode_int64(decryptor.decrypt(EY).to_string()) << endl;

    cout << "Noise : " << inherent_noise(EX, params, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(params).significant_bit_count() << " bits" << endl;
    cout << "Noise : " << inherent_noise(EY, params, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(params).significant_bit_count() << " bits" << endl;

};



//load matrixes from a file
//Tm: target user
//Tu: stranger size
//Fu: friend size
//n: item size
void LoadM(int MTu[Tu][n], int MFu[Fu][n], int sim[Fu], int user_num)
{
  ifstream fs("./ML100K/stranger_"+to_string(user_num)+"_"+to_string(Tu)+".dat");
  ifstream ff("./ML100K/friend_"+to_string(user_num)+"_"+to_string(Fu)+".dat");
  ifstream fw("./ML100K/similarity_"+to_string(user_num)+"_"+to_string(Fu)+".dat");

  if(fw.fail()|| fs.fail() || ff.fail())
    cout<<user_num<<" open failed"<<endl;

  for(int j=0; j<Fu; j++)
    {
        fw>>sim[j];
        //cout<<sim[j];
    }    
    //cout<<endl;

  for(int j=0; j<Fu; j++)
  for(int i=0; i<n; i++)
        {
            ff>>MFu[j][i];
            //cout<<MFu[j][i];
        }

  for(int j=0; j<Tu; j++)
  for(int i=0; i<n; i++)
        fs>>MTu[j][i];

  ff.close();
  fs.close();
  fw.close();
}

//sigle HE operation time cost
void Ttest(Encryptor encryptor,Decryptor decryptor,Evaluator evaluator, BalancedEncoder encoder, clock_t start)
{
    BigPoly Ent[n];

    start=clock();
    encryptor.encrypt( encoder.encode(12), Ent[0] );
    cout<<"size:"<<sizeof(Ent[0])<<endl;
    
    start=clock();
    encryptor.encrypt( encoder.encode(1), Ent[0] );
    cout<<"Time for enc:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;

    start=clock();
    encryptor.encrypt( encoder.encode(20), Ent[1] );
    cout<<"Time for enc:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;

    start=clock();
    evaluator.multiply(Ent[0],  Ent[1], Ent[2] );
    evaluator.multiply(Ent[0],  Ent[2], Ent[4] );
    cout<<"Time for multply:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    cout << "res: " << encoder.decode_int64(decryptor.decrypt(Ent[2])) << endl;

    start=clock();
    evaluator.multiply_plain(Ent[0],  encoder.encode(3), Ent[2] );
    cout<<"Time for multply_plain:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    cout << "res: " << encoder.decode_int64(decryptor.decrypt(Ent[2])) << endl;
    
    start=clock();
    evaluator.add(Ent[0],  Ent[1], Ent[2] );
    cout<<"Time for add:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;

    start=clock();
    evaluator.add_plain(Ent[0],  encoder.encode(3), Ent[2] );
    cout<<"Time for add_plain:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;

    start=clock();
    int num=encoder.decode_int64(decryptor.decrypt(Ent[0] ));
    cout<<"Time for dec:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    cout<<"num:"<<num<<endl;

    start=clock();
    int num2=encoder.decode_int64(decryptor.decrypt( Ent[4]));
    cout<<"Time for dec:"<<(clock()-start)/(double)CLOCKS_PER_SEC <<endl;
    cout<<"num2:"<<num2<<endl;
}