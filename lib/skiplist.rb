$LOAD_PATH << File.dirname(__FILE__)
require 'cskiplist'

class SkipList
  
  def initialize(p, levels)
    @sl = CSkipList.new(levels, p)
  end
  
  def insert(val)
    @sl.insert(val)
  end

  def include?(val)
    @sl.contains(val)
  end

  def delete(val)
    @sl.delete(val)
  end
end