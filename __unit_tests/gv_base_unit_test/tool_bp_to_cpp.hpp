namespace  tool_bp_to_cpp
{
	//--------------------------------------------
	//change the parameter declaration ; 
	//--------------------------------------------
	const char * test_string =
		" \
 NewVariables(65) = (VarName = \"Raymarch Velocity MID\", VarGuid = 3F1A905C43F6BC834DA6108A562D894B, VarType = (PinCategory = \"object\", PinSubCategoryObject = Class'\"/Script/Engine.MaterialInstanceDynamic\"'), FriendlyName = \"Raymarch Velocity MID\", Category = NSLOCTEXT(\"\", \"8486B646400EA00F81C6C9817315235B\", \"Debug\"), PropertyFlags = 65541) \n\r \
 NewVariables(66) = (VarName = \"Radial Force\", VarGuid = EAF7DB734CBA9EFB365B318B3570678B, VarType = (PinCategory = \"float\"), FriendlyName = \"Radial Force\", Category = NSLOCTEXT(\"\", \"0ACC9F4D438A5E1E79024AA7A6078AFD\", \"Seed Velocity Setup\"), PropertyFlags = 5)  \n\r \
 NewVariables(67) = (VarName = \"Noise Field Pos Offset\", VarGuid = C56B1ADC4203E52B661594B91F9A8DDA, VarType = (PinCategory = \"struct\", PinSubCategoryObject = ScriptStruct'\"/Script/CoreUObject.Vector\"'), FriendlyName = \"Noise Field Pos Offset\", Category = NSLOCTEXT(\"\", \"91D201364FEA0DBAB8E026BDCB2FB8A9\", \"Seed Velocity Setup\"), PropertyFlags = 5)  \n\r\
 NewVariables(68) = (VarName = \"Velocity Density Mask Amount\", VarGuid = 0844092A41041A51CF9708B1E24CE0A8, VarType = (PinCategory = \"float\"), FriendlyName = \"Velocity Density Mask Amount\", Category = NSLOCTEXT(\"\", \"DB782BD24E01994377ACF09836811EC5\", \"Debug\"), PropertyFlags = 5)  \n\r\
 NewVariables(69) = (VarName = \"Temperature Inset\", VarGuid = DB9EF22245A12B2603CFE8BFAFF7630F, VarType = (PinCategory = \"float\"), FriendlyName = \"Temperature Inset\", Category = NSLOCTEXT(\"\", \"528913324075216BB5F78B9816D970EB\", \"Seed Color Setup\"), PropertyFlags = 5)  \n\r\
 NewVariables(70) = (VarName = \"Skeletal Mesh Actors\", VarGuid = 944D547C4967F229E0DA11AEF6B0EC74, VarType = (PinCategory = \"object\", PinSubCategoryObject = Class'\"/Script/Engine.SkeletalMeshActor\"', ContainerType = Array), FriendlyName = \"Skeletal Mesh Actors\", Category = NSLOCTEXT(\"\", \"6CCD8FB141F73F9BB6C403B506134932\", \"Voxelization Setup\"), PropertyFlags = 2053) \n\r\
 NewVariables(71) = (VarName = \"CameraTransforms\", VarGuid = D47BDF134A3B8066DF683AACD3C59936, VarType = (PinCategory = \"struct\", PinSubCategoryObject = ScriptStruct'\"/Script/CoreUObject.Transform\"', ContainerType = Array), FriendlyName = \"Camera Transforms\", Category = NSLOCTEXT(\"\", \"4EC236964ED39333F4A550B2CBF498BC\", \"Voxelization Setup\"), PropertyFlags = 65541) ";

	void main(gvt_array<gv_string > &args)
	{
		using namespace gv::gv_lang_cpp;
		{
			gvt_lexer<gv_scanner_string> lexer;
			gv_string text = test_string;

			gv_string_tmp new_text;
			gv_string file_name = "none";
			if (args.size())
			{
				file_name = *args[0];
				gv_load_file_to_string(*file_name, text);
			}
			//illegal string in text file 
			text.replace_all("'", " ");
			lexer.load_string(text); 

			gv_string_tmp replace_prefix = "";
			gv_string_tmp ignore_prefix = "";
			gv_string_tmp new_file_name = "result.hpp";
			gvi_stream * ps = gv_global::fm->open_text_file_for_write(*new_file_name);
			lexer.enable_record(true);
			TOKEN token = (TOKEN)lexer.lex();
			lexer.enable_record(false);
			int scope_depth = 0;
			while (token)
			{
				if (token == TOKEN_ID && lexer.get_id() == "NewVariables")
				{
				
					lexer.strip_until_string("VarName");
					lexer.next_match(TOKEN_ASSIGNOP); 
					lexer.next_match(TOKEN_STRING); 
					gv_string var_name = lexer.get_string();
					var_name.replace_all(" ", ""); 
					gv_string_tmp prefix = ""; 
					gv_string_tmp type_name;
					gv_string_tmp final_type; 
					lexer.strip_until_string("PinCategory");
					lexer.next_match(TOKEN_ASSIGNOP);
					lexer.next_match(TOKEN_STRING);
					type_name= lexer.get_string();
					if (type_name == "object")
					{
						//VarType = (PinCategory = \"object\", PinSubCategoryObject = Class'\"/Script/Engine.SkeletalMeshActor\"', ContainerType = Array)
						lexer.next_match(TOKEN_COMMA);
						lexer.next_match_id("PinSubCategoryObject");
						lexer.next_match(TOKEN_ASSIGNOP);
						lexer.next_match_id("Class");
						lexer.next_match(TOKEN_STRING);
						gv_string_tmp class_name = lexer.get_string();
						int index;
						bool find = class_name.find('.', index);
						if (find) class_name.delete_string(class_name.begin(), index+1);
						gv_string_tmp prefix = "U"; 
						if (class_name.find_string("Actor")) {
							prefix = "A";
						}
						type_name = prefix+class_name + "*";
						
					}
					else if (type_name == "struct")
					{
						
						//VarType = (PinCategory = \"struct\", PinSubCategoryObject = ScriptStruct'\"/Script/CoreUObject.Transform\"', ContainerType = Array)
						lexer.next_match(TOKEN_COMMA);
						lexer.next_match_id("PinSubCategoryObject");
						lexer.next_match(TOKEN_ASSIGNOP);
						lexer.next_match_id("ScriptStruct");
						lexer.next_match(TOKEN_STRING);
						gv_string_tmp class_name = lexer.get_string();
						int index;
						bool find = class_name.find('.', index);
						if (find) class_name.delete_string(class_name.begin(), index + 1);
						gv_string_tmp prefix = "F";
						type_name = prefix+class_name ;
					}
					final_type = type_name;
					if (lexer.look_ahead_token() == TOKEN_COMMA)
					{
						lexer.next_match(TOKEN_COMMA);
						lexer.next_match_id("ContainerType");
						lexer.next_match(TOKEN_ASSIGNOP);
						lexer.next_match_id("Array");
						final_type = "TArray<";
						final_type << type_name << ">"; 
							//to do add other Container;
					}
					new_text << "UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Simulation )" << gv_endl;
					new_text << final_type << " " << var_name <<";"<< gv_endl;
					lexer.strip_this_line();
					token = (TOKEN)lexer.lex();
				}
				else{
					lexer.strip_this_line();
					token = (TOKEN)lexer.lex();
				}
				 
			}
			(*ps) << new_text;
			delete(ps);
		}
	}
}

