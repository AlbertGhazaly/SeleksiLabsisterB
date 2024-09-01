program Pascal;

type
  TMatrix = array of array of Double;

procedure SwapRows(var matrix: TMatrix; row1, row2, n: Integer);
var
  i: Integer;
  temp: Double;
begin
  for i := 0 to n - 1 do
  begin
    temp := matrix[row1][i];
    matrix[row1][i] := matrix[row2][i];
    matrix[row2][i] := temp;
  end;
end;

procedure GaussJordan(var matrix, inverse: TMatrix; n: Integer);
var
  i, j, k: Integer;
  pivot, factor: Double;
begin
  for i := 0 to n - 1 do
  begin
    if matrix[i][i] = 0 then
    begin
      for j := i + 1 to n - 1 do
      begin
        if matrix[j][i] <> 0 then
        begin
          SwapRows(matrix, i, j, n);
          SwapRows(inverse, i, j, n);
          Break;
        end;
      end;
    end;

    pivot := matrix[i][i];
    for j := 0 to n - 1 do
    begin
      matrix[i][j] := matrix[i][j] / pivot;
      inverse[i][j] := inverse[i][j] / pivot;
    end;

    for j := 0 to n - 1 do
    begin
      if j <> i then
      begin
        factor := matrix[j][i];
        for k := 0 to n - 1 do
        begin
          matrix[j][k] := matrix[j][k] - factor * matrix[i][k];
          inverse[j][k] := inverse[j][k] - factor * inverse[i][k];
        end;
      end;
    end;
  end;
end;

var
  inputFile: TextFile;
  outputFile: TextFile;
  n, i, j: Integer;
  matrix, inverse: TMatrix;
begin
  Assign(inputFile, 'input/input.txt');
  Assign(outputFile, 'output/pascal.txt');
  Reset(inputFile);
  Rewrite(outputFile);

  ReadLn(inputFile, n);
  SetLength(matrix, n, n);
  SetLength(inverse, n, n);

  for i := 0 to n - 1 do
  begin
    for j := 0 to n - 1 do
    begin
      Read(inputFile, matrix[i][j]);
      if i = j then
        inverse[i][j] := 1.0
      else
        inverse[i][j] := 0.0;
    end;
  end;

  GaussJordan(matrix, inverse, n);

  for i := 0 to n - 1 do
  begin
    for j := 0 to n - 1 do
    begin
      Write(outputFile, inverse[i][j]:0:6, ' ');
    end;
    WriteLn(outputFile);
  end;

  Close(inputFile);
  Close(outputFile);
end.
