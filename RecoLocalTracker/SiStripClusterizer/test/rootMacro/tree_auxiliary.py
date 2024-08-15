from array import array

typeMap = {
  'Int_t'     : 'i',
  'UInt_t'    : 'I',
  'Float_t'   : 'f',
  'Double_t'  : 'd',
  'Char_t'    : 'b',
  'UChar_t'   : 'B',
  'Long64_t'  : 'l',
  'ULong64_t' : 'L',
  'UShort_t'  : 'H'
}

def get_branch_array(tree_object, branch_name, maxlen = 1):
  branch_list = [ br for br in tree_object.GetListOfBranches() ]
  branch_list_filtered = list(filter(lambda br: br.GetName() == branch_name, branch_list))
  if len(branch_list_filtered) == 0 and 'dead' in tree_object.GetName():
      return []
  assert(len(branch_list_filtered) == 1)

  leaf_object = branch_list_filtered[0].GetLeaf(branch_name)
  leaf_type = leaf_object.GetTypeName()
  if leaf_type not in typeMap:
    raise ValueError('Unrecognized type found in branch %s: %s' % (branch_name, leaf_type))

  array_type = typeMap[leaf_type]
  array_unit = [0.] if array_type in ['f', 'd'] else [0]
  branch_array = array(array_type, array_unit * maxlen)
  tree_object.SetBranchAddress(branch_name, branch_array)

  return branch_array

