require 'lib/skiplist'

describe "SkipList" do
  it "should initialize" do
    sl = SkipList.new(0.25, 4)
  end

  it "should include? and insert" do
    sl = SkipList.new(0.25, 4)
    sl.include?('a').should be_false
    sl.include?('b').should be_false
    sl.include?('c').should be_false
    sl.insert("b")
    sl.insert("a")
    sl.insert("c")
    sl.include?('a').should be_true
    sl.include?('b').should be_true
    sl.include?('c').should be_true
  end
  
  it "should delete" do
    sl = SkipList.new(0.25, 4)
    sl.insert("b")
    sl.include?('b').should be_true
    sl.delete("b")
    sl.include?('b').should be_false
  
  end
end