function getLength(arr, n=0) {
	if(typeof arr === "number"){
		return n+1;
	}else{
		arr.forEach(elem =>{
			n=Number(getLength(elem, n));
		})
	}
	return n;
}

let Test = {
  assertEquals: (result, answer)=>{
    if(result===answer){
      console.log("passed");
    }else{
      console.log("Failed");
    }
  }
}

Test.assertEquals(getLength([1, [2,3]]), 3)
Test.assertEquals(getLength([1, [2, [3, 4]]]), 4)
Test.assertEquals(getLength([1, [2, [3, [4, [5, 6]]]]]), 6)
Test.assertEquals(getLength([1, 7, 8]), 3)
Test.assertEquals(getLength([2]), 1)
Test.assertEquals(getLength([2, [3], 4, [7]]), 4)
Test.assertEquals(getLength([2, [3, [5, 7]], 4, [7]]), 6)
Test.assertEquals(getLength([2, [3, [4, [5]]], [9]]), 5)
Test.assertEquals(getLength([]), 0)

