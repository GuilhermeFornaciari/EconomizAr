// ID da planilha aqui
var SS = SpreadsheetApp.openById('1yGlecNO4K6qJOA1iW0gpqWaGxp4s3CgB5TIviQJKNBY').getActiveSheet();
var str = "";


function doPost(e) {

  var parsedData;
  var result = {};
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Erro em separar os dados da request: " + f.message);
  }
   
  if (parsedData !== undefined){
    var flag = parsedData.format;
    if (flag === undefined){
      flag = 0;
    }
    //tentativa conserto
    var sheet = SS
    var dataArr = parsedData.values.split(","); // cria um vetor dos valores a serem publicados    
        
    var date_now = Utilities.formatDate(new Date(), 'GMT-4', "dd/MM/yyyy"); // pega a data atual
    var time_now = Utilities.formatDate(new Date(), 'GMT-4', "hh:mm:ss a"); // pega a hora atual
    
    
    
    
    
    var value0 = dataArr [0]; // value0 do código do arduino
    var value1 = dataArr [1]; // value1 do código do arduino
    var value2 = dataArr [2]; // value2 do código do arduino
    
    
    // le e executa o comando da "payload_base" na  string especificada no codigo do arduino, no caso, "insert"    
         
    var publish_array = new Array(); // create a new array
       
    publish_array [0] = date_now; // adiciona a data para a posição 0 em publish_array
    publish_array [1] = time_now; // adiciona a hora para a posição 1 em publish_array
    publish_array [2] = value0;   // adiciona o value0 do código do arduino para a posição 2 em publish_array
    publish_array [3] = value1;   // adiciona o value0 do código do arduino para a posição 2 em publish_array
    publish_array [4] = value2;   // adiciona o value0 do código do arduino para a posição 2 em publish_array
      
    
    //Postar itens na sheet1
    var newRow = sheet.getLastRow() + 1;
    var newRange = sheet.getRange(newRow, 1, 1, publish_array.length);
    newRange.setValues([publish_array]);
      
    str = "Successo"; // string que volta para o serial do arduino
    SpreadsheetApp.flush();
      
 
  
    
    return ContentService.createTextOutput(str);
  } // endif (parsedData !== undefined)
  
  else {
    return ContentService.createTextOutput("Erro! dados vazios ou em formato incorreto.");
  }
}
